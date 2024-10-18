#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_INSTRUCOES 1000 // Número total de instruções a serem geradas
#define OPCODES 2 // Número de diferentes operações suportadas (0 e 1 para soma e subtração)
#define MAX_ENDERECO 999 // Limite superior do endereço de memória 


// Gera 3 arquivos txt com diferentes niveis de repeticao(50%, 75% e 90%)
//Primeiro, gera os opcodes aleatorios e os endereços de forma aleatoria
//Depois, gera o nivel de repeticao das instrucoes
void gerarInstrucoes(int repeticao, const char* nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    int instrucao[4];
    int instrucoesGeradas[NUM_INSTRUCOES][4]; // Armazena instruções geradas
    int numGeradas = 0; // Contador de instruções geradas
    srand(time(NULL));

    for (int i = 0; i < NUM_INSTRUCOES; i++) {
        // Gera os opcodes aleatorios e os endereços de forma aleatoria
        instrucao[0] = rand() % OPCODES; // Gera um opcode aleatório (0 ou 1)
        instrucao[1] = rand() % MAX_ENDERECO;
        instrucao[2] = rand() % MAX_ENDERECO;
        instrucao[3] = rand() % MAX_ENDERECO;

        // Armazena a nova instrução
        instrucoesGeradas[numGeradas][0] = instrucao[0];
        instrucoesGeradas[numGeradas][1] = instrucao[1];
        instrucoesGeradas[numGeradas][2] = instrucao[2];
        instrucoesGeradas[numGeradas][3] = instrucao[3];
        numGeradas++;

        // Parte que gera o nivel de repeticao das instrucoes
        // Determina se a instrução atual deve ser uma repetição
        if (rand() % 100 < repeticao && numGeradas > 1) {
            int j = rand() % (numGeradas - 1); // Escolhe uma instrução anterior aleatoriamente
            fprintf(arquivo, "%d %d %d %d\n", instrucoesGeradas[j][0], instrucoesGeradas[j][1], instrucoesGeradas[j][2], instrucoesGeradas[j][3]);
        } else {
            fprintf(arquivo, "%d %d %d %d\n", instrucao[0], instrucao[1], instrucao[2], instrucao[3]);
        }
    }

    // Adiciona a instrução de parada
    fprintf(arquivo, "-1\n");

    fclose(arquivo);
    printf("Instruções geradas e salvas em %s\n", nomeArquivo);
}

int main() {
    gerarInstrucoes(50, "instrucoes_50.txt"); // Gera instruções com 50% de repetição
    gerarInstrucoes(75, "instrucoes_75.txt"); // Gera instruções com 75% de repetição
    gerarInstrucoes(90, "instrucoes_90.txt"); // Gera instruções com 90% de repetição

    return 0;
}

