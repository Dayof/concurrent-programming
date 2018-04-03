// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>

#define CAN 4
#define COZ 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t canibal_cond = PTHREAD_COND_INITIALIZER;

int REF_READY = 0, REF = 0;

void * consumir(void* id)
{
  int pid = *(int *)(id);

  while (1)
  {
    std::cout << "--- CANIBAL " << pid << " QUER CONSUMIR ---\n\n";

    pthread_mutex_lock(&mutex);

    if (REF_READY > 0)
    {
      int comer = rand() % REF_READY + 1;
      REF_READY = REF_READY - comer;
      std::cout << "--- CANIBAL " << pid << " CONSUMIU " << comer << " REFEIÇÕES ---\n\n";
      std::cout << "--- RESTAM " << REF_READY << " REFEIÇÕES ---\n\n";
    }
    else
    {
      std::cout << "--- CANIBAL " << pid << " CHAMA COZINHEIRO PARA PRODUZIR MAIS REFEIÇÕES ---\n\n";
      pthread_cond_signal(&cozinheiro_cond);
      std::cout << "--- CANIBAL " << pid << " AGUARDANDO PRODUÇÃO DAS REFEIÇÕES ---\n\n";
      pthread_cond_wait(&canibal_cond, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    sleep(1);
  }

  pthread_exit(0);
}

void * preparar(void* id)
{
  int pid = *(int *)(id);

  while (1)
  {
    pthread_mutex_lock(&mutex);

    std::cout << "--- COZINHEIRO PREPARANDO " << REF << " REFEIÇÕES ---\n\n";
    std::cout << "--- " << REF_READY+1 << " REFEIÇÕES PRONTAS ---\n\n";

    REF_READY++;
    sleep(1);

    while (REF_READY == REF)
    {
      std::cout << "--- COZINHEIRO ACORDA CANIBAIS PARA COMER ---\n\n";
      pthread_cond_broadcast(&canibal_cond);
      std::cout << "--- COZINHEIRO FOI DORMIR POIS JÁ PRODUZIU TODAS AS REFEIÇÕES ---\n\n";
      pthread_cond_wait(&cozinheiro_cond, &mutex);
      REF = rand() % 50;
    }

    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(0);
}

void create_threads()
{
  pthread_t canibais[CAN], cozinheiro[COZ];
  int *id;

  id = (int *) malloc(sizeof(int));
  *id = 0;
  if (pthread_create(&cozinheiro[0], NULL, preparar, (void *) (id)))
  {
    std::cout << "Erro na criacao do cozinheiro.\n\n";
    exit(1);
  }

  for (int i = 0; i < CAN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;

    if (pthread_create(&canibais[i], NULL, consumir, (void *) (id)))
    {
      std::cout << "Erro na criacao do canibal " << i << std::endl;
      exit(1);
    }
  }

  pthread_join(cozinheiro[0], NULL);
}

int main()
{
  // Comida inicial
  REF = rand() % 50;

  create_threads();

  return 0;
}
