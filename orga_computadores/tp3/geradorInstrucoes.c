#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_INSTRUCOES 500 // Número total de instruções a serem geradas
#define OPCODES 3 // Número de diferentes operações suportadas (0, 1 e 2 para soma, subtração e interrupção)
#define MAX_ENDERECO 999 // Limite superior do endereço de memória 
#define OPCODE_SOMA 0
#define OPCODE_SUB 1
#define OPCODE_INT 2
#define PROB_INT 20 // Probabilidade de gerar uma instrução de interrupção

// Gera 3 arquivos txt com diferentes níveis de repetição (50%, 75% e 90%)
// Primeiro, gera os opcodes aleatórios e os endereços de forma aleatória
// Depois, gera o nível de repetição das instruções
void gerarInstrucoes(int repeticao, const char* nomeArquivo, int seed) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }

    int instrucao[4];
    int instrucoesGeradas[NUM_INSTRUCOES][4]; // Armazena instruções geradas
    int numGeradas = 0; // Contador de instruções geradas
    srand(seed); // Inicializa a semente do gerador de números aleatórios

    for (int i = 0; i < NUM_INSTRUCOES; i++) {
        // Determina se a instrução atual será uma interrupção
        if (rand() % 100 < PROB_INT) {
            // Gera uma instrução de interrupção
            instrucao[0] = OPCODE_INT;
            instrucao[1] = -1; // Campos de endereço não utilizados
            instrucao[2] = -1;
            instrucao[3] = -1;
        } else {
            // Gera os opcodes aleatórios e os endereços de forma aleatória
            instrucao[0] = rand() % (OPCODES - 1); // Gera um opcode aleatório (0 ou 1)
            instrucao[1] = rand() % (MAX_ENDERECO + 1);
            instrucao[2] = rand() % (MAX_ENDERECO + 1);
            instrucao[3] = rand() % (MAX_ENDERECO + 1);
        }

        // Armazena a nova instrução
        instrucoesGeradas[numGeradas][0] = instrucao[0];
        instrucoesGeradas[numGeradas][1] = instrucao[1];
        instrucoesGeradas[numGeradas][2] = instrucao[2];
        instrucoesGeradas[numGeradas][3] = instrucao[3];
        numGeradas++;

        // Parte que gera o nível de repetição das instruções
        // Determina se a instrução atual deve ser uma repetição
        if (rand() % 100 < repeticao && numGeradas > 0) {
            int j = rand() % numGeradas; // Escolhe uma instrução anterior aleatoriamente
            fprintf(arquivo, "%d %d %d %d\n", instrucoesGeradas[j][0], instrucoesGeradas[j][1], instrucoesGeradas[j][2], instrucoesGeradas[j][3]);
        } else {
            fprintf(arquivo, "%d %d %d %d\n", instrucao[0], instrucao[1], instrucao[2], instrucao[3]);
        }
    }

    // Adiciona a instrução de parada
    fprintf(arquivo, "-1 -1 -1 -1\n");

    fclose(arquivo);
    printf("Instruções geradas e salvas em %s\n", nomeArquivo);
}

int main() {
    // Inicializa a semente do gerador de números aleatórios uma única vez
    srand(time(NULL));
    int seed = rand(); // Obtém uma semente aleatória

    gerarInstrucoes(50, "instrucoes_50.txt", seed); 
    gerarInstrucoes(75, "instrucoes_75.txt", seed + 1); 
    gerarInstrucoes(90, "instrucoes_90.txt", seed + 2);

    return 0;
}

