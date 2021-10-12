#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "timer.h"
#define T_BUFFER 10

int tamBloco;
FILE* arqEnt;
FILE* arqSai;
int nThreads;
int nLinhas;
int *buffer[T_BUFFER];
long limite;
int inc;
int out;
char* saida;
char* entrada;
int flagErro = 0;
int ciclos = 0;

sem_t bufferSlotVazio;
sem_t bufferSlotCheio;
pthread_mutex_t mutexA; // Condição de acesso aos slots do buffer
pthread_mutex_t mutexB; // Condição de acesso ao arquivo de saída
pthread_mutex_t mutexC;

FILE *trataArquivo(char* caminho, char* const tipo){
    FILE *arq;
    arq = fopen(caminho, tipo);
    return arq;
}

void lePrimeiraLinha(FILE* arq, int N){
    int *qNumeros;
    qNumeros = (int *)malloc(sizeof(int));
    fscanf(arq, "%d", qNumeros);
    nLinhas = *qNumeros/N;
    if(*qNumeros%N!=0){
        printf("Divisão entre quantidade de números e tamanho do bloco não tem resto igual a zero!\n");
        exit(0);
    }
}

int *inicializadorBloco(FILE *arq, int N){
    //cria o bloco
    int *bloco = (int*) malloc(sizeof(int)*N);
    for(int i=0; i<N; i++){
        fscanf(arq, "%d", bloco+i);
    }
    return bloco;
}

void quick_sort(int *array, int left, int right) {
    int i, j, x, y;
     
    i = left;
    j = right;
    x = array[(left + right) / 2];
     
    while(i <= j) {
        while(array[i] < x && i < right) {
            i++;
        }
        while(array[j] > x && j > left) {
            j--;
        }
        if(i <= j) {
            y = array[i];
            array[i] = array[j];
            array[j] = y;
            i++;
            j--;
        }
    }
     
    if(j > left) {
        quick_sort(array, left, j);
    }
    if(i < right) {
        quick_sort(array, i, right);
    }
}

void sequencialCode() {
    int k = 0;
    while(k<nLinhas){
        int *bloco = inicializadorBloco(arqEnt, tamBloco);
        quick_sort(bloco, 0, tamBloco-1);
        for(int i = 0; i < tamBloco; i++){
            fprintf(arqSai, "%d ", bloco[i]);
        }
        fprintf(arqSai, "\n");
        k++;
    }
}

void iniciaVars(char * nomeEntrada, char * nomeSaida){

    arqEnt = trataArquivo(nomeEntrada, "r");
    arqSai = trataArquivo(nomeSaida, "w");
    
    inc = 0;
    out = 0;

}

void terminaVars(){
    for (int i = 0; i < T_BUFFER; i++){
        free(buffer[i]);
    };

    fclose(arqEnt);
    fclose(arqSai);
}

int main(int argc, char **argv){
    double inicio, fim, delta;
    if (argc < 4) {
        fprintf(stderr, "Insira %s <Tamanho do bloco> <Nome do arquivo de entrada> <Nome do arquivo de saida>\n", argv[0]);
        exit(1);
    }
    tamBloco = atoi(argv[1]);
    entrada = argv[2];
    saida = argv[3];

    iniciaVars(entrada, saida);
    lePrimeiraLinha(arqEnt, tamBloco);
    GET_TIME(inicio);
    sequencialCode();
    GET_TIME(fim);
    terminaVars();

    delta = fim - inicio;
    printf("Tempo: %.6lf\n", delta);

    if(flagErro==0){
        printf("Arquivo gerado com sucesso!\n");
    }

    return 0;
}