#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int memoriaInstrucoes[1000][4];
int memoriaDados[1000];
int umaInstrucao[4];

void montarRam();  
void menu(); 
void maquina(); //contabiliza as vezes que o programa rodou e salva as instruçoes na memoriaDados
void maquinaInterpretada(int *umaInstrucao); //realiza: soma, subtração, guardar e trazer da memória
void incrementar(int endereco); // Incrementa o valor 1 ao valor salvo no endereço
int obterConteudoMemoria(int endereco); // Obtém o conteúdo de um endereço da RAM
void salvarNaMemoria(int valor, int endereco); // Salva um valor na memória RAM
void multiplicacao(int opcode, int endereco1, int endereco2, int endereco3);
void divisao(int opcode, int endereco1, int endereco2, int endereco3);
void exponencial(int opcode, int endereco1, int endereco2, int endereco3);
void programa(int *umaInstrucao); //Chama as funções mais complexas

int main() {
    montarRam();
    menu();
    while(1){
        maquina();
        programa(umaInstrucao);
    }
}

void montarRam() {
    srand(time(NULL));
    for (int i = 0; i < 1000; i++) {
        memoriaDados[i] = rand() % 100;
    }
    memoriaDados[108] = 0;
}

void menu() {
    printf("Comandos: \n \t -1 - Encerrar programa \n \t 0 - Soma  \n \t 1 - Subtração \n \t 4 -  Multiplicação \n \t 5 - Divisão \n \t 6 - Exponencial \n \t (Os comandos devem ter 4 números)\n");
}

void maquina() {
    int PC = obterConteudoMemoria(108);
    printf("Entre com uma instrução:\n");
    scanf("%d %d %d %d", &umaInstrucao[0], &umaInstrucao[1], &umaInstrucao[2], &umaInstrucao[3]);
    int opcode = umaInstrucao[0];

    switch (opcode) {
        case -1:{
            printf("Programa terminou!!\n\n");
            exit(0);
        }
        case 0: {
            printf("%d + %d = ", obterConteudoMemoria(umaInstrucao[1]), obterConteudoMemoria(umaInstrucao[2]));
            maquinaInterpretada(umaInstrucao);
            printf("%d \n", obterConteudoMemoria(umaInstrucao[3]));
            break;
        }
        case 1: {
            printf("%d - %d = ", obterConteudoMemoria(umaInstrucao[1]), obterConteudoMemoria(umaInstrucao[2]));
            maquinaInterpretada(umaInstrucao);
            printf("%d \n", obterConteudoMemoria(umaInstrucao[3]));
            break;
        }
        case 2: {
            maquinaInterpretada(umaInstrucao);
            printf("%d foi salvo no endereço %d\n", umaInstrucao[1], umaInstrucao[2]);
            break;
        }
        case 3: {
            maquinaInterpretada(umaInstrucao);
            printf("%d está salvo no endereço %d\n", obterConteudoMemoria(umaInstrucao[1]), umaInstrucao[1]);
            break;
        }

        int i = 0;
        salvarNaMemoria(0, 109);
        while (i < 4) {
            memoriaInstrucoes[PC][i] = umaInstrucao[i];
            incrementar(109);
            i = obterConteudoMemoria(109);
        }
        incrementar(108);
    }
}

void maquinaInterpretada(int *umaInstrucao) {
    int opcode = umaInstrucao[0];
    switch (opcode) {
        case 0: {
            // SOMAR
            int end1 = umaInstrucao[1];
            int end2 = umaInstrucao[2];
            int end3 = umaInstrucao[3];
            int conteudoRam1 = obterConteudoMemoria(end1);
            int conteudoRam2 = obterConteudoMemoria(end2);
            int soma = conteudoRam1 + conteudoRam2;
            salvarNaMemoria(soma, end3);
            break;
        }
        case 1: {
            // SUBTRAIR
            int end1 = umaInstrucao[1];
            int end2 = umaInstrucao[2];
            int end3 = umaInstrucao[3];
            int conteudoRam1 = obterConteudoMemoria(end1);
            int conteudoRam2 = obterConteudoMemoria(end2);
            int sub = conteudoRam1 - conteudoRam2;
            salvarNaMemoria(sub, end3);
            break;
        }
        case 2: {
            // LEVAR PARA memoriaDados
            int content = umaInstrucao[1];
            int end = umaInstrucao[2];
            memoriaDados[end] = content;
            break;
        }
        case 3: {
            // TRAZER DA memoriaDados
            int endereco = umaInstrucao[1];
            // Busca o valor no endereço especificado e coloca de volta na memória de dados
            umaInstrucao[2] = memoriaDados[endereco];
            break;
        }
    }
}

void incrementar(int endereco) {
    int incrementar[4] = {2, 1, 107, 0}; // Incrementa o valor no endereço
    maquinaInterpretada(incrementar);
    int incrementarInstrucao[4] = {0, endereco, 107, endereco};
    maquinaInterpretada(incrementarInstrucao);
}

int obterConteudoMemoria(int endereco) {
    int instrucao[4] = {3, endereco, 0, 0}; // Instrução de "trazer da memória"
    maquinaInterpretada(instrucao);
    return instrucao[2];
}

void salvarNaMemoria(int valor, int endereco) {
    int instrucao[4] = {2, valor, endereco, 0}; // Instrução de "guardar na memória"
    maquinaInterpretada(instrucao);
}

void multiplicacao(int opcode, int endereco1, int endereco2, int endereco3) {
    int conteudoRam2 = obterConteudoMemoria(endereco2);
    int i = 0;
    salvarNaMemoria(0, 110);
    salvarNaMemoria(0, endereco3);
    int contas[4] = {0, endereco1, endereco3, endereco3};
    while (i < conteudoRam2) {
        maquinaInterpretada(contas);
        incrementar(110);
        i = obterConteudoMemoria(110);
    }
}

void divisao(int opcode, int endereco1, int endereco2, int endereco3) {// Divisão
    int conteudoRam1 = obterConteudoMemoria(endereco1);
    int conteudoRam2 = obterConteudoMemoria(endereco2);
    int dividendo = conteudoRam1;
    if(conteudoRam2 == 0){
        printf("O número zero está salvo no endereço %d e não é possível dividir um número por 0\n", endereco2);
        return;
    }
    salvarNaMemoria(0, 110);
    int contas[4] = {1, endereco1, endereco2, endereco1};
    while (dividendo >= conteudoRam2) {
        maquinaInterpretada(contas);
        dividendo = obterConteudoMemoria(endereco1);
        incrementar(110);
    }
    int resultado = obterConteudoMemoria(110);
    salvarNaMemoria(resultado, endereco3);
    salvarNaMemoria(conteudoRam1, endereco1);
    printf("Resultado: %d / %d = %d ", conteudoRam1, conteudoRam2, resultado);
    if (dividendo != 0)
        printf(" sobra %d", dividendo);
    printf("\n");
}

void exponencial(int opcode, int endereco1, int endereco2, int endereco3) {
    int conteudoRam1 = obterConteudoMemoria(endereco1);
    int conteudoRam2 = obterConteudoMemoria(endereco2);
    if(conteudoRam2 == 0){
        printf("%d elevado a 0 = 1 \n", conteudoRam1);
        return;
    }
    salvarNaMemoria(1, 110);
    int i = 1, j = 0;
    int contas[4] = {0, 112, endereco3, endereco3};
    salvarNaMemoria(0, 111);
    salvarNaMemoria(conteudoRam1, endereco3);
    salvarNaMemoria(conteudoRam1, 112);
    while (i < conteudoRam2) {
        while(j < conteudoRam1-1) {
            maquinaInterpretada(contas);
            incrementar(111);
            j = obterConteudoMemoria(111);
        }
        salvarNaMemoria(0, 111);
        j = 0;
        salvarNaMemoria(obterConteudoMemoria(endereco3), 112);
        incrementar(110);
        i = obterConteudoMemoria(110);
        printf("%d elevado a %d = %d \t", conteudoRam1, i, obterConteudoMemoria(endereco3));
        if(i == 6 || i == 11 || i == 16 || i == 21 || i == 26 || i == 31 || i == 36 || i == 41 || i ==46 || i == 51){
            printf("\n");
        }
    }
    int resultado = obterConteudoMemoria(endereco3);
    printf("\nResultado: %d elevado a %d = %d\n", conteudoRam1, conteudoRam2, resultado);
}


void programa(int *umaInstrucao){
    switch (umaInstrucao[0]) {
        case 4: {
            multiplicacao(umaInstrucao[0], umaInstrucao[1], umaInstrucao[2], umaInstrucao[3]);
            int conteudoRam1 = obterConteudoMemoria(umaInstrucao[1]);
            int conteudoRam2 = obterConteudoMemoria(umaInstrucao[2]);
            int resultado = obterConteudoMemoria(umaInstrucao[3]);
            printf("Resultado: %d x %d = %d\n", conteudoRam1, conteudoRam2, resultado);
            break;
        }
        case 5:{
            divisao(umaInstrucao[0], umaInstrucao[1], umaInstrucao[2], umaInstrucao[3]);
            break;
        }
        case 6: {
            exponencial(umaInstrucao[0], umaInstrucao[1], umaInstrucao[2], umaInstrucao[3]);
            break;
        }
    }
}