#include <stdio.h>
#include <stdlib.h>
#include "tratador_interrupcao.h"

#define TAM_BLOCO 4

// Função para salvar o contexto atual da CPU
void salvar_contexto(ContextoCPU* contexto, int PC_atual) {
    contexto->PC = PC_atual;
    printf("Contexto salvo: PC = %d, Registrador1 = %d, Registrador2 = %d\n", 
           contexto->PC, contexto->registrador1, contexto->registrador2);
}

// Função para restaurar o contexto da CPU
void restaurar_contexto(ContextoCPU* contexto, int* PC_atual) {
    *PC_atual = contexto->PC;
    printf("Contexto restaurado: PC = %d, Registrador1 = %d, Registrador2 = %d\n", 
           *PC_atual, contexto->registrador1, contexto->registrador2);
}

// Função que representa o tratador de interrupção
void tratar_interrupcao(MemoriaExterna* memoria, ContextoCPU* contexto, int* PC_atual) {
    printf("Interrupção ocorrida. Tratando interrupção...\n");

    // Salva o contexto atual
    salvar_contexto(contexto, *PC_atual);

    printf("Executando tratador de interrupção...\n");
    
    // Operação: Leitura de um bloco da memória externa
    int palavras_leitura[TAM_BLOCO];
    int endereco_bloco_leitura = 0; // Endereço do bloco a ser lido
    if (ler_bloco_memoria_externa(memoria, endereco_bloco_leitura, palavras_leitura) == 0) {
        printf("Dados do bloco %d lido da memória externa: ", endereco_bloco_leitura);
        for(int i = 0; i < TAM_BLOCO; i++) {
            printf("%d ", palavras_leitura[i]);
        }
        printf("\n");
    } else {
        printf("Falha ao ler o bloco %d da memória externa.\n", endereco_bloco_leitura);
    }

    printf("Executando instruções específicas no tratador de interrupção...\n");
    for (int i = 0; i < TAM_BLOCO; i++) {
        // Incrementa os dados lidos da memória
        palavras_leitura[i] += 10; //Incrementa 10
        printf("Modificando dado no bloco %d, posição %d: %d\n", endereco_bloco_leitura, i, palavras_leitura[i]);
    }

    // Escrita do bloco modificado de volta na memória externa
    if (gravar_bloco_memoria_externa(memoria, endereco_bloco_leitura, palavras_leitura) == 0) {
        printf("Dados gravados no bloco %d da memória externa: ", endereco_bloco_leitura);
        for(int i = 0; i < TAM_BLOCO; i++) {
            printf("%d ", palavras_leitura[i]);
        }
        printf("\n");
    } else {
        printf("Falha ao escrever no bloco %d da memória externa.\n", endereco_bloco_leitura);
    }

    // Instrução que modifica um registrador
    contexto->registrador1 += 5; // Incrementa o registrador1 em 5
    printf("Registrador1 modificado: %d\n", contexto->registrador1);

    // Restaura o contexto após o tratamento
    restaurar_contexto(contexto, PC_atual);
    printf("Interrupção tratada. Retornando ao programa principal.\n");
}




