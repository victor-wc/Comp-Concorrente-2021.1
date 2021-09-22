
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4 //numero de threads

//variáveis globais
sem_t condA; //condição das frases 2 e 3
sem_t condB, condC; //condições da frase 4

//Tarefas das threads
void* frase_1(void* arg) {
	printf("Seja bem-vindo!\n");
	sem_post(&condA);
	pthread_exit(NULL);
}

void* frase_2(void* arg) {
	sem_wait(&condA);
	printf("Fique a vontade.\n");
	sem_post(&condA);
	sem_post(&condB);
	pthread_exit(NULL);
}

void* frase_3(void* arg) {
	sem_wait(&condA);
	printf("Sente-se por favor.\n");
	sem_post(&condA);
	sem_post(&condC);
	pthread_exit(NULL);
}

void* frase_4(void* arg) {
	sem_wait(&condB);
	sem_wait(&condC);
	printf("Volte sempre!\n");
	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    pthread_t threads[NTHREADS];

    //Inicializa os semáforos
    sem_init (&condA, 0, 0);
	sem_init (&condB, 0, 0);
	sem_init (&condC, 0, 0);

	if(pthread_create(&threads[0], NULL, frase_2, NULL)) {
		printf("ERRO: pthread_create(0)\n");
		exit(-1);
	}
	if(pthread_create(&threads[1], NULL, frase_1, NULL)) {
		printf("ERRO: pthread_create(1)\n");
		exit(-1);
	}
	if(pthread_create(&threads[2], NULL, frase_4, NULL)) {
		printf("ERRO: pthread_create(2)\n");
		exit(-1);
	}
	if(pthread_create(&threads[3], NULL, frase_3, NULL)) {
		printf("ERRO: pthread_create(3)\n");
		exit(-1);
	}

	//Espera todas as threads completarem
    for(int i = 0; i < NTHREADS; i++) {
		if(pthread_join(threads[i], NULL)) {
			printf("ERRO: pthread_join(%d)\n", i);
			exit(-1);
		}
  	}

	//Libera os semáforos
	if(sem_destroy(&condA)) {
		printf("ERRO: sem_destroy(A)\n");
		exit(-1);
	}
	if(sem_destroy(&condB)) {
		printf("ERRO: sem_destroy(B)\n");
		exit(-1);
	}
	if(sem_destroy(&condC)) {
		printf("ERRO: sem_destroy(C)\n");
		exit(-1);
	}

	return 0;
}
