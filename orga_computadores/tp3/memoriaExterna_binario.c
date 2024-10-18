#include "memoriaExterna_binario.h"
#include <stdlib.h>
#include <string.h>

// Função para criar e inicializar a memória externa
MemoriaExterna* criar_memoria_externa(const char* nome_arquivo, int num_blocos) {
    if (num_blocos > MAX_BLOCOS) {
        fprintf(stderr, "Número de blocos excede o máximo permitido (%d).\n", MAX_BLOCOS);
        return NULL;
    }
    
    //Alocacao de memoria para a estrutura MemoriaExterna
    MemoriaExterna* memoria = (MemoriaExterna*)malloc(sizeof(MemoriaExterna));
    if (!memoria) {
        perror("Erro ao alocar memória para MemoriaExterna");
        return NULL;
    }

    // Abre o arquivo binário para leitura e escrita; cria se não existir
    memoria->arquivo = fopen(nome_arquivo, "wb+");
    if (!memoria->arquivo) {
        perror("Erro ao criar/abrir o arquivo de memória externa");
        free(memoria);
        return NULL;
    }
    printf("Arquivo de memória externa \"%s\" aberto com sucesso.\n", nome_arquivo);
    // Inicializa o arquivo com blocos vazios (palavras = 0)
    Bloco bloco_vazio;
    memset(bloco_vazio.palavras, 0, sizeof(bloco_vazio.palavras));

    for (int i = 0; i < num_blocos; i++) {
        fwrite(&bloco_vazio, sizeof(Bloco), 1, memoria->arquivo);
    }

    memoria->num_blocos = num_blocos; //Define o número de blocos na estrutura MemoriaExterna
    fflush(memoria->arquivo); // Garante que os dados sejam gravados

    return memoria;
}

// Função para liberar a memória externa e fechar o arquivo
void liberar_memoria_externa(MemoriaExterna* memoria) {
    if (memoria) {
        if (memoria->arquivo) {
            fclose(memoria->arquivo);
        }
        free(memoria);
    }
}

// Função para gravar um bloco na memória externa
int gravar_bloco_memoria_externa(MemoriaExterna* memoria, int indice, const int* palavras) {
    if (indice < 0 || indice >= memoria->num_blocos) { //Validação do Índice
        fprintf(stderr, "Índice de bloco inválido: %d\n", indice);
        return -1;
    }

    
    long deslocamento = indice * sizeof(Bloco); // Calcula o deslocamento no arquivo onde o bloco deve ser escrito 
    if (fseek(memoria->arquivo, deslocamento, SEEK_SET) != 0) { //Usa fseek para mover o ponteiro do arquivo para a posição correta
        perror("Erro ao posicionar no bloco para escrita");
        return -1;
    }

    //Copia os dados das palavras fornecidas para o bloco.palavras usando memcpy
    Bloco bloco;
    memcpy(bloco.palavras, palavras, sizeof(int) * TAM_BLOCO);


    size_t escritos = fwrite(&bloco, sizeof(Bloco), 1, memoria->arquivo);
    fflush(memoria->arquivo); // Garante que os dados sejam gravados

    if (escritos != 1) { //Verificação da Escrita
        fprintf(stderr, "Falha ao escrever o bloco %d na memória externa.\n", indice);
        return -1;
    }
    return 0;
}

// Função para ler um bloco da memória externa
int ler_bloco_memoria_externa(MemoriaExterna* memoria, int indice, int* palavras) {
    if (indice < 0 || indice >= memoria->num_blocos) { //Validação do Índice
        fprintf(stderr, "Índice de bloco inválido: %d\n", indice);
        return -1;
    }

    // Calcula o deslocamento no arquivo
    long deslocamento = indice * sizeof(Bloco);
    if (fseek(memoria->arquivo, deslocamento, SEEK_SET) != 0) { //Posicionamento no Arquivo
        perror("Erro ao posicionar no bloco para leitura");
        return -1;
    }

    Bloco bloco;
    size_t lidos = fread(&bloco, sizeof(Bloco), 1, memoria->arquivo);
    if (lidos != 1) {
        fprintf(stderr, "Falha ao ler o bloco %d da memória externa.\n", indice);
        return -1;
    }

    memcpy(palavras, bloco.palavras, sizeof(int) * TAM_BLOCO);

    return 0;
}

// Função para converter um inteiro para uma sequência de bytes (big-endian)
void int_para_binario(int valor, unsigned char* buffer) {
    buffer[0] = (valor >> 24) & 0xFF;
    buffer[1] = (valor >> 16) & 0xFF;
    buffer[2] = (valor >> 8) & 0xFF;
    buffer[3] = valor & 0xFF;
}

// Função para converter uma sequência de bytes para inteiro (big-endian)
int binario_para_int(const unsigned char* buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}



