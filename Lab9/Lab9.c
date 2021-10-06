#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10

sem_t fullBuffer, emptySlot;
sem_t mutexProd, mutexCons;
int in;
int buffer[N];


void insert(int el, int id){
    sem_wait(&mutexProd);
    sem_wait(&emptySlot);
    printf("Produtor %d: trabalhando na posição %d do buffer\n", id, in);
    buffer[in] = el;

    in = (in + 1) % N;
    if (in == 0) {
        printf("Produtor %d: buffer cheio\n", id);
        sem_post(&fullBuffer);
    }

    sem_post(&mutexProd);
}

void rmv(int id) {
    int item;
    sem_wait(&fullBuffer);
    printf("Consumidor %d: consumindo o buffer \n", id);
    for(int i = 0; i < N; i++) {
        item = buffer[i];
        printf("Retirando %d do buffer \n", item);
        sem_post(&emptySlot);
    }
}

void* producer(void * arg) {
    int *id = (int *) arg;
    while(1) {
        insert(rand() % 5 + *id, *id);
        sleep(1);
    }
    free(arg);
    pthread_exit(NULL);
}

void* consumer(void * arg) {
    int *id = (int *) arg;
    while(1) {
        rmv(*id);
        sleep(1);
    }
    free(arg);
    pthread_exit(NULL);
}


void init_sem(sem_t *mutexProd, sem_t *fullBuffer, sem_t *emptySlot){
    sem_init(mutexProd, 0, 1);
    sem_init(fullBuffer, 0, 0);
    sem_init(emptySlot, 0, N);
}

void thread_cycle(pthread_t *tids, int produtores, int consumidores){
    int *id[produtores + consumidores];

    for (int i = 0; i < (consumidores + produtores); i++) {
        id[i] = malloc(sizeof(int));
        if(id[i] == NULL){
            exit(-1);
        }
        *id[i] = i + 1;
    }

    //criacao das threads
    for (int i = 0; i < produtores; i++) {
        if (pthread_create(tids + i, NULL, producer, (void *) id[i])){
            printf("erro na criação das threads\n");
            exit(-1); 
        } 
    }

    for (int i = 0; i < consumidores; i++) {
        if (pthread_create(tids + i + produtores, NULL, consumer, (void *) id[i + produtores])){
            printf("erro na criação das threads\n");
            exit(-1); 
        }
    }

    //join das threads
    for (int i = 0; i < (produtores + consumidores); i++) {
        if (pthread_join(*(tids + i), NULL)) {
            fprintf(stderr, "erro no join das threads \n");
            exit(2);
        }
    }

}


int main(int argc, char *argv[]) {
    int produtores, consumidores;
    pthread_t *tids; // identificadores das threads no sistema

    if (argc < 3) {
        printf("Entrada correta: %s <quantidade-de-produtores> <quantidade-de-consumidores> \n", argv[0]);
        return 1;
    }

    produtores = atoi(argv[1]);
    consumidores = atoi(argv[2]);

    tids = (pthread_t *) malloc(sizeof(pthread_t) * (produtores + consumidores));
    if (tids == NULL) {
        printf("erro na alocacao de memoria para tids \n");
        exit(2);
    }

    in = 0;

    srand(time(NULL));

    for (int i = 0; i < N; i ++){
        buffer[i] = 0;
    }

    init_sem(&mutexProd, &fullBuffer, &emptySlot);

    thread_cycle(tids, produtores, consumidores);

    sem_destroy(&mutexProd);
	sem_destroy(&fullBuffer);
	sem_destroy(&emptySlot);
    free(tids);

    return 0;
}