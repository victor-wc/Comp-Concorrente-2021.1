//Soma todos os elementos de um vetor de inteiro
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

int nthreads;
int *array; 
pthread_mutex_t mutex;
pthread_cond_t condition;
int bloqueadas = 0;

void barreira(int nthreads, long long int id) {
    pthread_mutex_lock(&mutex); //inicio secao critica
    if (bloqueadas == (nthreads-1)) {
      printf("Atenção! Thread %lld chegou na barreira\n",id); 
      //ultima thread a chegar na barreira
      pthread_cond_broadcast(&condition);
      bloqueadas=0;
    } else {
      printf("Atenção! Thread %lld foi bloqueada\n", id);
      bloqueadas++;
      pthread_cond_wait(&condition, &mutex);
    }
    pthread_mutex_unlock(&mutex); //fim secao critica
}

//fluxo das threads
void * tarefa(void * arg){
    long long id = (long int) arg; //identificador da thread
    int *somatorio;

    somatorio = (int *) malloc(sizeof(int));
    if (somatorio == NULL) {
      fprintf(stderr,  "ERRO--malloc\n");
      exit(3);
    }
    *somatorio = 0;

    for(int i = 0; i<nthreads; i++){
      printf("%da iteracao: \nThread %lld\n", i + 1, id);
      for(int k = 0; k<nthreads; k++){
        *somatorio += array[k];
      }


      barreira(nthreads, id);

      array[id] = rand()%10;

      barreira(nthreads, id);
    }


    pthread_exit((void *) somatorio);
}


void iniThreads(pthread_t **tid, int numThreads) {
  // Aloca memoria para as estruturas de dados
  (*tid) = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if (*tid == NULL) {
    fprintf(stderr,  "ERRO--malloc\n");
    exit(2);
  }

  // Cria as threads
  for(long int i = 0; i < nthreads; i++) {
    if (pthread_create((*tid) + i, NULL, tarefa, (void *) i)) {
      fprintf(stderr,  "ERRO--malloc\n");
      exit(3);
    }
  }

}

void checaCorretude(int *array){
  int checagem = array[0];
  for(int i = 0; i<nthreads; i++){
    printf("Resultado: Thread %d obteve %d\n",i,array[i]);
    if(array[i] != checagem){
      printf("Os valores são diferentes\n");
      exit(1);
    }
  }
  printf("Todos os valores são iguais\n");
}

void terminaThread(pthread_t *tid,int **array){
  int *resultado;
  for(int i = 0; i < nthreads; i++) {
    if(pthread_join(*(tid + i), (void **) &resultado)){
      printf("Erro no join das threads \n");
      exit(2);
    }
    (*array)[i] = *resultado;
  }
}


//fluxo princial
int main(int argc, char *argv[]){
    pthread_t *tid;
    int *somatorioArray;
    //recebe os parametros de entrada (numero de threads)
    if(argc < 2){
        fprintf(stderr, "Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }

    //Converte o argumento para inteiro
    nthreads = atoi(argv[1]);

    srand(time(NULL));
    
    //Aloca memoria para o vetor
    array = (int *) malloc(sizeof(int) * nthreads);
    if (array == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      exit(2);
    }

    //preenche o vetor  de entrada
    for(long int i=0;i<nthreads;i++){
        array[i] = rand()%10;
        printf("Valor %ld do vetor: %d\n", i+1, array[i]);
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);

    //criar as threads
    iniThreads(&tid, nthreads);
    terminaThread(tid, &array);
    checaCorretude(array);

    //Desaloca as variáveis
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    //libere as areas de memoria alocadas
    free(array);
    free(tid);
    return 0;
}