//Soma todos os elementos de um vetor de inteiro
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

long long nElem; //dimensao do vetor de entrada
int nthreads; //numero de threads
float *array; //vetor de entrada

typedef struct{
    float maxEl;
    float minEl;
} arrayValue;


//fluxo das threads
void * tarefa(void * arg){
    long long id = (long int) arg; //identificador da thread
    long long int tamBloco = nElem/nthreads; //tamanho do bloco de cada thread
    long long int  ini = id * tamBloco; //elemento inicial do bloco da thread
    long long int fim; //elemento final(nao processado) do bloco da thread

    arrayValue *values;
    values = (arrayValue *)malloc(sizeof(arrayValue));

    if(values == NULL){
      fprintf(stderr, "Erro na alocacao da estrutura\n"); 
      exit(1);
    }

    if(id == nthreads - 1) fim = nElem;
    else fim = ini+ tamBloco; //trata o resto se houver
    //soma os elementos do bloco da thread

    values->maxEl = array[ini];
    values->minEl = array[ini];
    for(long long int i=ini; i<fim; i++){
        if(array[i] > values->maxEl){
            values->maxEl = array[i];
        } else if (array[i] < values->minEl){
            values->minEl = array[i];   
        }
    }
    //retorna o resultado da soma local
    pthread_exit((void *) values);
}

void tarefaSequencial(float **inputArray, arrayValue *seqValue, int numElements) {
  for (long long int i = 1; i < numElements; i++) {
    if ((*inputArray)[i] > seqValue->maxEl) {
      seqValue->maxEl = (*inputArray)[i];
    }
    if ((*inputArray)[i] < seqValue->minEl) {
      seqValue->minEl = (*inputArray)[i];
    }
  }
}

void iniThreads(pthread_t **tid, arrayValue *concValue, int numThreads ) {
  // Aloca memoria para as estruturas de dados
  (*tid) = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if (*tid == NULL) {
    fprintf(stderr, "Erro na alocacao de memoria para tid \n");
    exit(2);
  }

  // Cria as threads
  for(long int i = 0; i < nthreads; i++) {
    if (pthread_create((*tid) + i, NULL, tarefa, (void *) i)) {
      fprintf(stderr, "Erro na criacao das threads \n");
      exit(3);
    }
  }

  arrayValue *threadsValue; // valor de retorno das threads

  for(long int i = 0; i < nthreads; i++) {
    if (pthread_join(*((*tid) + i), (void **) &threadsValue)) {
      fprintf(stderr, "Erro no join das threads \n");
      exit(4);
    }

    if (threadsValue->maxEl > concValue->maxEl) {
      concValue->maxEl = threadsValue->maxEl;
    }

    if (threadsValue->minEl < concValue->minEl) {
      concValue->minEl = threadsValue->minEl;
    }
  }
}

void checaIgualdade(arrayValue *seqValue, arrayValue *concValue){
    if(seqValue->maxEl == concValue->maxEl && seqValue->minEl == concValue->minEl){
        printf("Tudo certo!\n");
    } else{
        printf("Algo nao confere...\n");
    }
}

//fluxo princial
int main(int argc, char *argv[]){
    arrayValue seqValue, concValue;
    double ini, fim; //tomada de tempo
    pthread_t *tid;
    //recebe os parametros de entrada (dimensao do vetor, numero de threads)
    if(argc < 3){
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }

    //Converte os argumentos para inteiro
    nElem = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    //aloca vetor de entrada
    array = (float*) malloc(sizeof(float)*nElem);
    if(array == NULL){
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    //preenche o vetor  de entrada
    for(long int i=0;i<nElem;i++)
        array[i] = 1000.0 * (float)rand() / (float)RAND_MAX;

    //atribuindo valor das estruturas ao primeiro item do array
    concValue.maxEl = seqValue.maxEl = array[0];
    concValue.minEl = seqValue.minEl = array[0];
    //checagem sequencial dos elementos
    GET_TIME(ini);
    tarefaSequencial(&array, &seqValue, nElem);
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim-ini);
    //criar as threads
    GET_TIME(ini);
    iniThreads(&tid, &concValue, nthreads);
    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim-ini);

    //Verifica se tudo está correto
    checaIgualdade(&seqValue, &concValue);

    //libere as areas de memoria alocadas
    free(array);
    free(tid);
    return 0;
}