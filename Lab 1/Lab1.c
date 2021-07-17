#include<stdio.h>
#include<pthread.h>

#define TAM 10000

int vetor[TAM];
int vetorCheck[TAM];

//funcao que a thread ira executar
void * tarefa (void * arg) {
    int ident = * (int *) arg;
    if(ident == 1){
        for(int i = 1; i<=TAM; i++){
            if(vetor[i] % 2 != 0){
                vetor[i] = vetor[i]*vetor[i];
            }
        }
    }
    if(ident == 2){
        for(int i = 1; i<=TAM; i++){
            if(vetor[i] % 2 == 0){
                vetor[i] = vetor[i]*vetor[i];
            }
        }
    }
    pthread_exit(NULL);
}

//preenche o array de checagem e o array das threads
void preencheArrays(){
    for(int i = 0; i<TAM; i++){
        vetor[i] = i;
        vetorCheck[i] = i;
    }
}

//eleva o array de checagem
void elevaArrayCheck(){
    for(int i = 0; i<TAM; i++){
        vetorCheck[i] = vetorCheck[i]*vetorCheck[i];
    }
}

//checa se o array mocado e o array construido pelas threads batem
int checaIgualdade(){
    int correto;
    for(int i = 0; i<TAM; i++){
        if(vetorCheck[i] != vetor[i]){
            correto = 0;
        }
        else{
            correto = 1;
        }
    }
    return correto;
}

//funcao principal
int main(void){
    pthread_t tid[2]; //identificador da thread no sistema
    int ident[2];  //identificador local da thread

    preencheArrays();

    //criação das threads
    for(int i = 1; i<=2; i++ ){
        ident[i-1] = i;
        if(pthread_create(&tid[i-1], NULL, tarefa, (void *)&ident[i-1]))
            printf("ERRO -- pthread_create\n");
    }

    elevaArrayCheck();

    //espera as threads terminarem
    for(int i = 0; i<2; i++ ){
        if(pthread_join(tid[i], NULL))
            printf("ERRO -- pthread_create\n");
    }

    //printa resposta da checagem para o usuário
    if(checaIgualdade() == 0){
        printf("Algum valor não bateu");
        printf("\n");
    } else{
        printf("Todos os valores conferem!");
        printf("\n");
    }

    //imprimir o vetor resultante
    // for(int i = 0; i<TAM; i++ ){
    //     printf("%d ", vetor[i]);
    // }
    // printf("\n");
    return 0;

}