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
    int* qNumeros;
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

void *produtora(void *arg) {
    int k = 0;
    while(k<nLinhas){
        sem_wait(&bufferSlotVazio);
        int *bloco = inicializadorBloco(arqEnt, tamBloco);
        buffer[inc] = bloco;
        inc = (inc + 1) % T_BUFFER;
        sem_post(&bufferSlotCheio);
        k++;
    }

    pthread_exit(NULL);
}

void *consumidora(void *args) {
    const long tId = (long) args;
    int bloco[tamBloco];
    while(1){
        // Condição de parada final
        printf("thread %ld\n", tId);
        if(ciclos == nLinhas){
            break;
        }
        ciclos++;
        printf("aqui 2 ciclos:%d\n", ciclos);

        sem_wait(&bufferSlotCheio);

        // Acesso ao slot do buffer - Apenas uma thread pode acessar por vez
        pthread_mutex_lock(&mutexA);
        for(int i=0; i<tamBloco; i++){
            bloco[i] = buffer[out][i];
        }
        out = (out + 1) % T_BUFFER;
        pthread_mutex_unlock(&mutexA);
        sem_post(&bufferSlotVazio);

        // Acesso ao arquivo - Apenas uma thread pode acessar por vez
        pthread_mutex_lock(&mutexB);
        quick_sort(bloco, 0, tamBloco-1);
        for(int i = 0; i < tamBloco; i++){
            fprintf(arqSai, "%d ", bloco[i]);
        }
        fprintf(arqSai, "\n");
        pthread_mutex_unlock(&mutexB);
    }

    pthread_exit(NULL);
}

void criaThreads(pthread_t *tidP, pthread_t *tidC){
    for (long i = 0; i < 1; i++) {
        if(pthread_create(&tidP[i], NULL, produtora, (void *) i)) {
            flagErro = 1;
            printf("ERRO 1\n");
            exit(2);
        }
    }
    for (long i = 1; i < nThreads; i++) {
        if(pthread_create(&tidC[i], NULL, consumidora, (void *) i)) {
            flagErro = 1;
            printf("ERRO 2\n");
            exit(3);
        }
    }
}

void encerraThreads(pthread_t *tidP, pthread_t *tidC){
    for (int i = 0; i < 1; i++) {
        if(pthread_join(tidP[i], NULL)){
            flagErro = 1;
            printf("ERRO 3\n");
            exit(4);
        }
    }
    for (int i = 1; i < nThreads; i++) {
        if(pthread_join(tidC[i], NULL)){
            flagErro = 1;
            printf("ERRO 4\n");
            exit(4);
        }
    }
}

void iniciaVars(char * nomeEntrada, char * nomeSaida){
    sem_init(&bufferSlotCheio, 0, 0);
    sem_init(&bufferSlotVazio, 0, T_BUFFER);
    pthread_mutex_init(&mutexA, NULL);
    pthread_mutex_init(&mutexB, NULL);
    pthread_mutex_init(&mutexC, NULL);

    arqEnt = trataArquivo(nomeEntrada, "r");
    arqSai = trataArquivo(nomeSaida, "w");
    
    inc = 0;
    out = 0;

}

void terminaVars(){
    for (int i = 0; i < T_BUFFER; i++){
        free(buffer[i]);
    };

    sem_destroy(&bufferSlotVazio);
    sem_destroy(&bufferSlotCheio);
    pthread_mutex_destroy(&mutexA);
    pthread_mutex_destroy(&mutexB);

    fclose(arqEnt);
    fclose(arqSai);
}

int main(int argc, char **argv){
    pthread_t *tidP, *tidC;
    double inicio, fim, delta;
    if (argc < 5) {
        fprintf(stderr, "Insira %s <Quantidade de threads> <Tamanho do bloco> <Nome do arquivo de entrada> <Nome do arquivo de saida>\n", argv[0]);
        exit(1);
    }
    nThreads = atoi(argv[1]);
    tamBloco = atoi(argv[2]);
    entrada = argv[3];
    saida = argv[4];

    tidP = (pthread_t *) malloc(sizeof(pthread_t) * (1));
    tidC = (pthread_t *) malloc(sizeof(pthread_t) * (nThreads));
    
    iniciaVars(entrada, saida);
    lePrimeiraLinha(arqEnt, tamBloco);
    GET_TIME(inicio);
    criaThreads(tidP, tidC);
    encerraThreads(tidP, tidC);
    GET_TIME(fim);
    terminaVars();

    delta = fim - inicio;
    printf("Tempo: %.6lf\n", delta);

    if(flagErro==0){
        printf("Arquivo gerado com sucesso!\n");
    }

    return 0;
}