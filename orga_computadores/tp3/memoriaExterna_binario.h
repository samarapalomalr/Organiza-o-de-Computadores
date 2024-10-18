// memoriaExterna_binario.h

#ifndef MEMORIA_EXTERNA_BINARIO_H
#define MEMORIA_EXTERNA_BINARIO_H

#include <stdio.h>

// Define o número de palavras por bloco e o número máximo de blocos
#define TAM_BLOCO 4
#define MAX_BLOCOS 1000

// Estrutura que representa um bloco na memória externa
typedef struct {
    int palavras[TAM_BLOCO]; // 4 palavras por bloco
} Bloco;

// Estrutura que representa a memória externa
typedef struct {
    FILE* arquivo;            // Ponteiro para o arquivo binário
    int num_blocos;           // Número total de blocos na memória
} MemoriaExterna;

// Funções para manipulação da memória externa
MemoriaExterna* criar_memoria_externa(const char* nome_arquivo, int num_blocos);
void liberar_memoria_externa(MemoriaExterna* memoria);
int gravar_bloco_memoria_externa(MemoriaExterna* memoria, int indice, const int* palavras);
int ler_bloco_memoria_externa(MemoriaExterna* memoria, int indice, int* palavras);

// Funções de conversão entre tipos de dados e binário
void int_para_binario(int valor, unsigned char* buffer);
int binario_para_int(const unsigned char* buffer);

#endif // MEMORIA_EXTERNA_BINARIO_H






