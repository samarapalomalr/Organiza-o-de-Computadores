// tratador_interrupcao.h
#ifndef TRATADOR_INTERRUPCAO_H
#define TRATADOR_INTERRUPCAO_H

#include "memoriaExterna_binario.h"

// Estrutura para representar o contexto da CPU (simplificado)
typedef struct {
    int PC;        // Program Counter
    // Adicione outros registradores conforme necessário
    int registrador1;
    int registrador2;
    // ...
} ContextoCPU;

// Declaração da função do tratador de interrupção
void tratar_interrupcao(MemoriaExterna* memoria, ContextoCPU* contexto, int* PC_atual);

// Funções para salvar e restaurar contexto
void salvar_contexto(ContextoCPU* contexto, int PC_atual);
void restaurar_contexto(ContextoCPU* contexto, int* PC_atual);

#endif // TRATADOR_INTERRUPCAO_H


