#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

pthread_mutex_t lock_writter_file = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_reader_file = PTHREAD_MUTEX_INITIALIZER;

#define W_N 3
#define R_N 3

pthread_t all[W_N + R_N];
int nl = 0;

void *writters (void *arg)
{
  int i = *((int *) arg);

  while (1)
  {
    sleep(1);
    // process data
    pthread_mutex_lock(&lock_writter_file);
    // write data
    std::cout << "---- ESCREVENDO NO ARQUIVO ---- " << i << std::endl;
    sleep(3);
    std::cout << "-- FIM DA ESCRITA -- " << i << std::endl;
    pthread_mutex_unlock(&lock_writter_file);
    sleep(3);
  }

  pthread_exit(0);
}

void *readers (void *arg)
{
  int i = *((int *) arg);

  while (1)
  {
    sleep(3);
    pthread_mutex_lock(&lock_reader_file);
    nl++;
    if (nl == 1) pthread_mutex_lock(&lock_writter_file);
    pthread_mutex_unlock(&lock_reader_file);
    // read data
    std::cout << "---- LENDO DADOS ---- " << i << std::endl;
    sleep(3);
    std::cout << "-- FIM DA LEITURA --" << i << std::endl;
    pthread_mutex_lock(&lock_reader_file);
    nl--;
    if (nl == 0) pthread_mutex_lock(&lock_writter_file);
    pthread_mutex_unlock(&lock_reader_file);
    // process data
    sleep(3);
  }

  pthread_exit(0);
}

void create_readers_and_writters()
{
  for (int i = 0; i < R_N ; i++) {
        int *id_r = (int *) malloc(sizeof(int));
        int *id_w = (int *) malloc(sizeof(int));
        *id_r = i;
        *id_w = i + W_N;
        pthread_create(&all[i], NULL, readers, (void *) (id_r));
        pthread_create(&all[i+W_N], NULL, writters, (void *) (id_w));
  }
}

int main()
{
  // create readers and writters
  create_readers_and_writters();

  pthread_join(all[0], NULL);

  return 0;
}
