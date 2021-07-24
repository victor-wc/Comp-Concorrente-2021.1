#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *mat1; //primeira matriz quadrada
float *vet; //segunda matriz quadrada
float *res; //matriz resultado da multiplicacao
int qthreads; //numero de threads
int tam; //dimensao da matriz quadrada

//funcao que as threads executarao
void * multiplicaMatriz(void *arg) {
	int id = *(int*)arg;
	for(int i = id; i < tam; i+=qthreads) {
		for(int j = 0; j < tam; j++) {
			for(int k = 0; k < tam; k++) {
				res[i*tam + j] += mat1[i*tam + k]*vet[k*tam + j];
			}
		}
	}
	pthread_exit(NULL);
}

//checa se o valor está corrreto
int checaIgualdade(){
	int correto = 1;
	for(int i = 0; i < tam; i++) {
		for(int j = 0; j < tam; j++) {
			if(res[i*tam + j] != tam) {
				correto = 0;
			}
		}
	}
    return correto;
}


int main(int argc, char* argv[]) {
	pthread_t *tid; //identificadores das threads no sistema
	double inicio, fim, delta; //valores para registrar o tempo gasto no codigo

	//leitura e avaliacao dos parametros de entrada
	GET_TIME(inicio);
	if(argc < 3) {
		printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
		return -1;
	}
	tam = atoi(argv[1]);
	qthreads = atoi(argv[2]) > tam ? tam : atoi(argv[2]); //qthreads precisa ser menor que a dimensao
	int ident[qthreads]; //identificador local da thread

	//alocacao de memoria para as estruturas de dados
	mat1 = (float *) malloc(sizeof(float)*tam*tam);
	if(mat1 == NULL) {
		printf("ERRO - malloc\n");
		return -1;
	}
	vet = (float *) malloc(sizeof(float)*tam*tam);
	if(vet == NULL) {
		printf("ERRO - malloc\n");
		return -1;
	}
	res = (float *) malloc(sizeof(float)*tam*tam);
	if(res == NULL) {
		printf("ERRO - malloc\n");
		return -1;
	}

	//inicializacao das estruturas de dados de entrada e saida
	for(int i = 0; i < tam; i++) {
		for(int j = 0; j < tam; j++) {
			mat1[i*tam + j] = 1;
			vet[i*tam + j] = 1;
			res[i*tam + j] = 0;
		}
	}


	GET_TIME(fim);
	delta = fim - inicio;
	printf("Tempo inicializacao: %lfs\n", delta);

	//alocacao das estruturas
	GET_TIME(inicio);
	tid = (pthread_t*) malloc(sizeof(pthread_t)*qthreads);
	if(tid == NULL) {
		puts("ERRO - malloc");
		return -1;
	}	

	//criacao das threads
	for(int i = 0; i < qthreads; i++) {
		ident[i] = i;
		if(pthread_create(&tid[i], NULL, multiplicaMatriz, (int*) &ident[i])) {
			puts("ERRO - pthread_create");
			return -1;
		}
	}

	//espera pelo termino das threads
	for(int i = 0; i < qthreads; i++) {
		pthread_join(*(tid + i), NULL);
	}
	GET_TIME(fim);
	delta = fim - inicio;
	printf("Tempo multiplicacao: %lfs\n", delta);


    //Armazena o valor da checagem
    int check = checaIgualdade();

	//liberacao da memoria
	GET_TIME(inicio);
	free(mat1);
	free(vet);
	free(res);
	free(tid);
	GET_TIME(fim);
	delta = fim - inicio;
	printf("Tempo finalizacao: %lfs\n", delta);

    //Printar a checagem
    if(check == 0){
        printf("Algum valor não bateu");
        printf("\n");
    } else{
        printf("Todos os valores conferem!");
        printf("\n");
    }

	return 0;
}