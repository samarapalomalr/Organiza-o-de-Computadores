// UCM E CPU AQUI NO ARQUIVO
// Contém a parte do gerenciamento de dados nas memórias

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_INSTRUCOES 1000
#define CONFIGURACOES 1
#define TAM_RAM 1024
#define NUM_CONJUNTOS 16 
#define BLCS_POR_CONJUNTO 4
#define TAM_BLOCO 4

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
        return;
    }
    insereCache(cache, endereco, valor);
}

// Busca um bloco nas caches L1, L2, e L3
int UCM_busca(int endereco, Cache *cache1, Cache *cache2, Cache *cache3, RAM *ram) {
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
    return -1;
}

// Executa uma lista de instruções
void executarInstrucoes(int instrucoes[][4], int numInstrucoes, Cache *cache1, Cache *cache2, Cache *cache3, RAM *ram) {
    for (int i = 0; i < numInstrucoes; i++) {
        int opcode = instrucoes[i][0];
        int end1 = instrucoes[i][1];
        int end2 = instrucoes[i][2];
        int end3 = instrucoes[i][3];

        // Os valores dos operandos são buscados nas caches
        int val1 = UCM_busca(end1, cache1, cache2, cache3, ram);
        int val2 = UCM_busca(end2, cache1, cache2, cache3, ram);

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

        // Depois, o resultado da operação (soma ou subtração) é salvo na RAM
        ram->blocos[ajustarEndereco(end3)] = resultado;
    }
}

// Lê instruções de um arquivo
int lerInstrucoesDeArquivo(char *nomeArquivo, int instrucoes[][4]) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return 0;
    }

    int numInstrucoes = 0;
    while (fscanf(arquivo, "%d %d %d %d", &instrucoes[numInstrucoes][0],
                  &instrucoes[numInstrucoes][1],
                  &instrucoes[numInstrucoes][2],
                  &instrucoes[numInstrucoes][3]) != EOF) {
        numInstrucoes++;
    }

    fclose(arquivo);
    return numInstrucoes;
}

// Função para exibir as estatísticas em forma de tabela
void exibirEstatisticas() {
    printf("\n=================== Estatísticas ===================\n");
    printf("| Tipo        | Hits | Misses | Taxa de Hit    |\n");
    printf("---------------------------------------------------\n");
    printf("| Cache L1   | %d   | %d     | %.2f            |\n", cache1Hits, cache1Misses, 
           (double)cache1Hits / (cache1Hits + cache1Misses) * 100);
    printf("| Cache L2   | %d   | %d     | %.2f            |\n", cache2Hits, cache2Misses, 
           (double)cache2Hits / (cache2Hits + cache2Misses) * 100);
    printf("| Cache L3   | %d   | %d     | %.2f            |\n", cache3Hits, cache3Misses, 
           (double)cache3Hits / (cache3Hits + cache3Misses) * 100);
    printf("| RAM        | %d   | %d     | %.2f            |\n", ramHits, ramMisses, 
           (double)ramHits / (ramHits + ramMisses) * 100);
    printf("---------------------------------------------------\n");
    printf("Tempo de Execução: %.2f segundos\n", tempoExecucao);
    printf("===================================================\n");
}

int main(int argc, char *argv[]) {
    Cache cache1, cache2, cache3;
    RAM ram;
    inicializarCache(&cache1);
    inicializarCache(&cache2);
    inicializarCache(&cache3);
    inicializarRAM(&ram);

    // Ler instruções do arquivo
    int instrucoes[MAX_INSTRUCOES][4];
    int numInstrucoes = lerInstrucoesDeArquivo("instrucoes_75.txt", instrucoes);

    clock_t inicio = clock(); // Início da contagem do tempo

    executarInstrucoes(instrucoes, numInstrucoes, &cache1, &cache2, &cache3, &ram);

    clock_t fim = clock(); // Fim da contagem do tempo
    tempoExecucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // Exibir estatísticas
    exibirEstatisticas();

    liberarCache(&cache1);
    liberarCache(&cache2);
    liberarCache(&cache3);
    liberarRAM(&ram);

    return 0;
}

