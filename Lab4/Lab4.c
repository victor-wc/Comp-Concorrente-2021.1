
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4 //numero de threads

//Variaveis globais
int x = 0;
pthread_mutex_t mutex;
pthread_cond_t condA; //condicao das frases 2 e 3
pthread_cond_t condB; //condicao da frase 4

//Tarefas das threads
void* frase_1(void* arg) {
	pthread_mutex_lock(&mutex);
	x++;
	printf("Seja bem-vindo!\n");
	pthread_cond_broadcast(&condA);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void* frase_2(void* arg) {
	pthread_mutex_lock(&mutex);
	if(x == 0) {
		pthread_cond_wait(&condA, &mutex);
	}
	printf("Fique a vontade.\n");
	x++;
	if(x == 3) {
		pthread_cond_signal(&condB);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void* frase_3(void* arg) {
	pthread_mutex_lock(&mutex);
	if(x == 0) {
		pthread_cond_wait(&condA, &mutex);
	}
	printf("Sente-se por favor.\n");
	x++;
	if(x == 3) {
		pthread_cond_signal(&condB);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void* frase_4(void* arg) {
	pthread_mutex_lock(&mutex);
	if(x < 3) {
		pthread_cond_wait(&condB, &mutex);
	}
	printf("Volte sempre!\n");
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    pthread_t threads[NTHREADS];

    //Inicializa o mutex e a variavel de condicao
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&condA, NULL);
	pthread_cond_init (&condB, NULL);

    //Cria as threads
    pthread_create(&threads[0], NULL, frase_2, NULL);
	pthread_create(&threads[1], NULL, frase_1, NULL);
    pthread_create(&threads[2], NULL, frase_4, NULL);
    pthread_create(&threads[3], NULL, frase_3, NULL);

	//Espera todas as threads completarem
    for (int i = 0; i < NTHREADS; i++) {
      pthread_join(threads[i], NULL);
    }

    ///Desaloca variaveis e termina
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condA);
    pthread_cond_destroy(&condB);
	return 0;
}
