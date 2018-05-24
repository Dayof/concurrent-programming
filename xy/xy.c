#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

pthread_mutex_t lx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ly = PTHREAD_MUTEX_INITIALIZER;
int x = 0, y = 0;

void * P1(void * arg){
    printf("ENTROU P1 x = %d, y = %d\n", x, y);
    pthread_mutex_lock(&lx);
    printf("P1 LOCK X x = %d, y = %d\n", x, y);
    x=x+1;
    printf("P1 x=x+1 x = %d, y = %d\n", x, y);
    x=x+y;
    printf("P1 x=x+y x = %d, y = %d\n", x, y);
    pthread_mutex_unlock(&lx);
    printf("P1 UNLOCK X x = %d, y = %d\n", x, y);
}

void * P2(void * arg){
    printf("ENTROU P2 x = %d, y = %d\n", x, y);
    pthread_mutex_lock(&ly);
    printf("P2 LOCK Y x = %d, y = %d\n", x, y);
    y=y+1;
    printf("P2 y=y+1 x = %d, y = %d\n", x, y);
    y=y+x;
    printf("P2 y=y+x x = %d, y = %d\n", x, y);
    pthread_mutex_unlock(&ly);
    printf("P2 UNLOCK Y x = %d, y = %d\n", x, y);
}

int main() {
   pthread_t a[2];
   int i, *id;

   printf("BEFORE THREADS x = %d, y = %d\n", x, y);
   pthread_create(&a[0], NULL, P1, NULL);
   pthread_create(&a[1], NULL, P2, NULL);
   sleep(1);
   printf("AFTER THREADS x = %d, y = %d\n", x, y);

   return 0;
}
