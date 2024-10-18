// tp3.c
// UCM E CPU AQUI NO ARQUIVO
//Contem a parte do gerenciamento de dados nas memorias

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "memoriaExterna_binario.h"
#include "tratador_interrupcao.h"

#define MAX_INSTRUCOES 1000
#define CONFIGURACOES 1
#define TAM_RAM 1024
#define NUM_CONJUNTOS 16 
#define BLCS_POR_CONJUNTO 4
#define TAM_BLOCO 4
#define MAX_BLOCOS 1000
typedef struct {
    int tag;
    int valid;
    int data; // Armazena o valor do bloco
} CacheBlock;

typedef struct {
    CacheBlock blocos[NUM_CONJUNTOS][BLCS_POR_CONJUNTO];
} Cache;

typedef struct {
    int *blocos;
} RAM;

int cache1Hits = 0, cache1Misses = 0;
int cache2Hits = 0, cache2Misses = 0;
int cache3Hits = 0, cache3Misses = 0;
int ramHits = 0, ramMisses = 0;      
int hdHits = 0, hdMisses = 0;        
double tempoExecucao;

// Inicialização de Estruturas de Memória
void inicializarCache(Cache *cache) {
    for (int i = 0; i < NUM_CONJUNTOS; i++) {
        for (int j = 0; j < BLCS_POR_CONJUNTO; j++) {
            cache->blocos[i][j].valid = 0;
            cache->blocos[i][j].tag = -1; // Inicializa com -1 indicando vazio
            cache->blocos[i][j].data = -1;
        }
    }
}

void inicializarRAM(RAM *ram) {
    ram->blocos = (int *)malloc(TAM_RAM * sizeof(int));
    if (ram->blocos == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a RAM.\n");
        exit(1);
    }
    for (int i = 0; i < TAM_RAM; i++) {
        ram->blocos[i] = -1; // Inicializa com -1 indicando vazio
    }
}

void liberarCache(Cache *cache) {
    for (int i = 0; i < NUM_CONJUNTOS; i++) {
        for (int j = 0; j < BLCS_POR_CONJUNTO; j++) {
            cache->blocos[i][j].valid = 0;
            cache->blocos[i][j].tag = -1;
            cache->blocos[i][j].data = -1;
        }
    }
}

void liberarRAM(RAM *ram) {
    free(ram->blocos);
}

// Garante que o endereço esteja no intervalo da RAM
int ajustarEndereco(int endereco) {
    return endereco % TAM_RAM;
}

// Mapeamento associativo por conjunto: Cada cache é dividida em vários conjuntos e cada conjunto há blocos

// Um endereço de memória é mapeado para um conjunto na cache 
int calcularConjunto(int endereco) {
    return (endereco / BLCS_POR_CONJUNTO) % NUM_CONJUNTOS;
}

// Função para atualizar a posição do bloco para refletir a política LRU
void atualizarLRU(Cache *cache, int conjunto, int pos) {
    if (pos == 0) return; // Já é o mais recentemente usado
    CacheBlock temp = cache->blocos[conjunto][pos];
    for (int i = pos; i > 0; i--) {
        cache->blocos[conjunto][i] = cache->blocos[conjunto][i - 1];
    }
    cache->blocos[conjunto][0] = temp;
}

// Busca um bloco na cache. Se o bloco for encontrado (hit), atualiza a posição para LRU e retorna o valor armazenado na cache
int buscaCache(Cache *cache, int endereco, int *hits, int *misses) {
    int conjunto = calcularConjunto(endereco);
    for (int i = 0; i < BLCS_POR_CONJUNTO; i++) {
        if (cache->blocos[conjunto][i].valid && cache->blocos[conjunto][i].tag == endereco) {
            (*hits)++;
            // Atualiza a posição para refletir a política LRU
            atualizarLRU(cache, conjunto, i);
            // Retorna o valor do bloco na cache
            return cache->blocos[conjunto][i].data;
        }
    }
    (*misses)++;
    return -1;
}

// Insere um bloco na cache seguindo a política LRU
void insereCache(Cache *cache, int endereco, int valor) {
    int conjunto = calcularConjunto(endereco);
    // Verifica se há espaço vazio
    for (int i = 0; i < BLCS_POR_CONJUNTO; i++) {
        if (!cache->blocos[conjunto][i].valid) {
            cache->blocos[conjunto][i].valid = 1;
            cache->blocos[conjunto][i].tag = endereco;
            cache->blocos[conjunto][i].data = valor; // Armazena o valor do bloco
            atualizarLRU(cache, conjunto, i);
            return;
        }
    }
    // Se não há espaço, substitui o bloco menos recentemente usado (último)
    cache->blocos[conjunto][BLCS_POR_CONJUNTO - 1].tag = endereco;
    cache->blocos[conjunto][BLCS_POR_CONJUNTO - 1].data = valor; // Armazena o valor do bloco
    atualizarLRU(cache, conjunto, BLCS_POR_CONJUNTO - 1);
}

// Simula o carregamento de dados da RAM quando ocorre um miss nas caches
void moverDaRAMParaCache(Cache *cache, int endereco, RAM *ram) {
    int valor = ram->blocos[ajustarEndereco(endereco)];
    if (valor == -1) {
        // Caso o dado não esteja na RAM, tenta carregar da memória externa
        // Isso será tratado na função UCM_busca
        return;
    }
    insereCache(cache, endereco, valor);
}

// Busca um bloco nas caches L1, L2, e L3
// Se o bloco for encontrado numa cache de nível mais baixo (L2 ou L3), ele é movido para as caches de níveis superiores
// Se o bloco não estiver em nenhuma cache, ele é carregado da RAM para a cache L3 e depois movido para L2 e L1
int UCM_busca(int endereco, Cache *cache1, Cache *cache2, Cache *cache3, RAM *ram, MemoriaExterna* memoria, ContextoCPU* contexto, int* PC_atual) {
    (void)contexto; // Suprimindo o aviso de parâmetro não utilizado
    (void)PC_atual; // Suprimindo o aviso de parâmetro não utilizado

    int resultado;

    // Busca na Cache L1
    resultado = buscaCache(cache1, endereco, &cache1Hits, &cache1Misses);
    if (resultado != -1) return resultado;

    // Busca na Cache L2
    resultado = buscaCache(cache2, endereco, &cache2Hits, &cache2Misses);
    if (resultado != -1) {
        // Move para Cache L1
        insereCache(cache1, endereco, resultado);
        return resultado;
    }

    // Busca na Cache L3
    resultado = buscaCache(cache3, endereco, &cache3Hits, &cache3Misses);
    if (resultado != -1) { 
        // Move para Cache L2 e L1
        insereCache(cache2, endereco, resultado);
        insereCache(cache1, endereco, resultado);
        return resultado; 
    }

    // Verifica na RAM
    if (ram->blocos[ajustarEndereco(endereco)] != -1) {
        ramHits++; // **Incrementa hits na RAM**
        resultado = ram->blocos[ajustarEndereco(endereco)];
        // Move o bloco da RAM para as caches
        insereCache(cache3, endereco, resultado);
        insereCache(cache2, endereco, resultado);
        insereCache(cache1, endereco, resultado);
        return resultado;
    }

    ramMisses++;

    // Verifica na Memória Externa (HD)
    int palavras[TAM_BLOCO];
    if (ler_bloco_memoria_externa(memoria, endereco, palavras)) {
        printf("Bloco %d não encontrado na memória externa (HD).\n", endereco);
        hdMisses++; // **Incrementa misses no HD**
        return -1;
    }

    hdHits++; // **Incrementa hits no HD**

    // Atualiza a RAM com os dados da Memória Externa
    for (int i = 0; i < TAM_BLOCO; i++) {
        ram->blocos[ajustarEndereco(endereco)] = palavras[i];
    }

    // Move o bloco da RAM para as caches
    insereCache(cache3, endereco, palavras[0]); // Supondo que palavras[0] é o valor relevante
    insereCache(cache2, endereco, palavras[0]);
    insereCache(cache1, endereco, palavras[0]);

    return ram->blocos[ajustarEndereco(endereco)];
}

void UCM_salva(int endereco, int valor, RAM *ram, MemoriaExterna* memoria) {
    ram->blocos[ajustarEndereco(endereco)] = valor;
    // Também grava na Memória Externa (HD)
    int palavras[TAM_BLOCO];
    palavras[0] = valor;
    for(int i = 1; i < TAM_BLOCO; i++) {
        palavras[i] = 0;
    }
    gravar_bloco_memoria_externa(memoria, endereco, palavras);
    hdHits++; // **Considerando que a gravação no HD foi bem-sucedida**
}

// Executa uma lista de instruções
void executarInstrucoes(int instrucoes[][4], int numInstrucoes, Cache *cache1, Cache *cache2, Cache *cache3, RAM *ram, MemoriaExterna* memoria, ContextoCPU* contexto, int* PC_atual) {

    for (int i = 0; i < numInstrucoes; i++) {
        int opcode = instrucoes[i][0];
        int end1 = instrucoes[i][1];
        int end2 = instrucoes[i][2];
        int end3 = instrucoes[i][3];

        if (opcode == -1) { // Instrução de parada
            printf("Instrução de parada encontrada. Encerrando execução.\n");
            break;
        }

        if (opcode == 2) { // Interrupção
            printf("Opcode de interrupção detectado.\n");
            tratar_interrupcao(memoria, contexto, PC_atual);
            continue; // Pula a execução da interrupção como instrução
        }

        // Os valores dos operandos são buscados nas caches
        int val1 = UCM_busca(end1, cache1, cache2, cache3, ram, memoria, contexto, PC_atual);
        int val2 = UCM_busca(end2, cache1, cache2, cache3, ram, memoria, contexto, PC_atual);

        if (val1 == -1 || val2 == -1) {
            printf("Erro ao buscar blocos necessários para a operação.\n");
            continue;
        }

        int resultado;
        if (opcode == 0) {  // Soma
            resultado = val1 + val2;
            printf("Operação de soma executada\n");
        } else if (opcode == 1) {  // Subtração
            resultado = val1 - val2;
            printf("Operação de subtração executada\n");
        } else {
            printf("Opcode desconhecido: %d. Ignorando instrução.\n", opcode);
            continue;
        }

        // Depois, o resultado da operação (soma ou subtração) é salvo na RAM e na Memória Externa (HD)
        UCM_salva(end3, resultado, ram, memoria);
    }
}

// Lê instruções de um arquivo e armazena em um array
int lerInstrucoes(char* arquivo, int instrucoes[][4]) {
    FILE *file = fopen(arquivo, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de instruções.\n");
        return 0;
    }
    int i = 0;
    while (i < MAX_INSTRUCOES && fscanf(file, "%d", &instrucoes[i][0]) != EOF) {
        if (instrucoes[i][0] != -1) {
            if (fscanf(file, "%d %d %d", &instrucoes[i][1], &instrucoes[i][2], &instrucoes[i][3]) != 3) {
                printf("Erro ao ler os parâmetros da instrução.\n");
                break;
            }
        }
        i++;
    }
    fclose(file);
    return i;
}

// Imprime a tabela de resultados
void imprimirTabelaResultados(int configuracaoAtual, int numInstrucoes) {
    (void)numInstrucoes; // Suprimindo o aviso de parâmetro não utilizado

    float taxaC1 = (cache1Hits + cache1Misses) ? ((float)cache1Hits / (cache1Hits + cache1Misses)) * 100 : 0.0;
    float taxaC2 = (cache2Hits + cache2Misses) ? ((float)cache2Hits / (cache2Hits + cache2Misses)) * 100 : 0.0;
    float taxaC3 = (cache3Hits + cache3Misses) ? ((float)cache3Hits / (cache3Hits + cache3Misses)) * 100 : 0.0;
    float taxaRAM = (ramHits + ramMisses) ? ((float)ramHits / (ramHits + ramMisses)) * 100 : 0.0;
    float taxaHD = (hdHits + hdMisses) ? ((float)hdHits / (hdHits + hdMisses)) * 100 : 0.0;

    printf("+-----------------------+---------+---------+---------+---------+---------+\n");
    printf("| Configuração %d       | Cache 1 | Cache 2 | Cache 3 |   RAM   |   HD    |\n", configuracaoAtual);
    printf("+-----------------------+---------+---------+---------+---------+---------+\n");
    printf("| Hits/ Misses         | %3d/%3d | %3d/%3d | %3d/%3d | %3d/%3d | %3d/%3d |\n", 
           cache1Hits, cache1Misses, 
           cache2Hits, cache2Misses, 
           cache3Hits, cache3Misses, 
           ramHits, ramMisses,
           hdHits, hdMisses);
    printf("| Taxa de Acerto       | %6.2f%% | %6.2f%% | %6.2f%% | %6.2f%% | %6.2f%% |\n",
           taxaC1, taxaC2, taxaC3, taxaRAM, taxaHD);
    printf("+-----------------------+---------+---------+---------+---------+---------+\n");
}

int main(int argc, char* argv[]) {
    // Inicialização
    srand(time(NULL));

    // Criar e inicializar as caches e RAM
    Cache cache1, cache2, cache3;
    inicializarCache(&cache1);
    inicializarCache(&cache2);
    inicializarCache(&cache3);
    RAM ram;
    inicializarRAM(&ram);

    // Criar e inicializar a memória externa (HD)
    const char* nome_arquivo_memoria = "memoria_externa.bin";
    int num_blocos_memoria = MAX_BLOCOS;
    MemoriaExterna* memoria = criar_memoria_externa(nome_arquivo_memoria, num_blocos_memoria);
    if (!memoria) {
        fprintf(stderr, "Falha ao criar a memória externa (HD).\n");
        liberarCache(&cache1);
        liberarCache(&cache2);
        liberarCache(&cache3);
        liberarRAM(&ram);
        return 1;
    }

    // Inicializar contexto da CPU
    ContextoCPU contexto;
    contexto.PC = 0;
    contexto.registrador1 = 0;
    contexto.registrador2 = 0;


    // Variável para controlar o Program Counter
    int PC_atual = 0;

    // Carregar instruções de um arquivo gerado pelo gerador_instrucoes.c
    const char* nome_arquivo_instrucao = "instrucoes_75.txt";
    if (argc > 1) {
        nome_arquivo_instrucao = argv[1];
    }

    int instrucoes[MAX_INSTRUCOES][4];
    int numInstrucoes = lerInstrucoes((char*)nome_arquivo_instrucao, instrucoes);

    clock_t inicio = clock();


    executarInstrucoes(instrucoes, numInstrucoes, &cache1, &cache2, &cache3, &ram, memoria, &contexto, &PC_atual);

    clock_t fim = clock();
    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    imprimirTabelaResultados(CONFIGURACOES, numInstrucoes);
    printf("Tempo de Execução: %.2f segundos\n", tempoExecucao);

    liberarCache(&cache1);
    liberarCache(&cache2);
    liberarCache(&cache3);
    liberarRAM(&ram);
    liberar_memoria_externa(memoria);

    return 0;
}




