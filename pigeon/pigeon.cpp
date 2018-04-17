// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <semaphore.h>

#define USERS 30
#define CARD_LIMIT 20

void * user(void *id);
void * pigeon(void *id);

sem_t pigeon_free;
sem_t cond;
sem_t fill_bag;
int cards_counter = 0;

void * user(void* id)
{
  int pid = *(int *)(id);

  std::cout << "--- USUÁRIO " << pid << " QUER ESCREVER CARTA ---\n\n";

  while (1)
  {
    sem_wait(&fill_bag);
    sem_wait(&cond);

    cards_counter++;
    std::cout << "--- USUÁRIO " << pid << " ESCREVENDO CARTA "
              << cards_counter << " ---\n\n";
    sleep(1);

    if (cards_counter == CARD_LIMIT)
    {
        sem_post(&pigeon_free);
        std::cout << "--- SACOLA CHEIA ---\n\n";
    }

    sem_post(&cond);
  }
  pthread_exit(0);
}

void * pigeon(void* id)
{
  while (1)
  {
    std::cout << "--- POMBO PREPARADO PARA RECEBER E LEVAR AS CARTAS ---\n\n";

    sem_wait(&pigeon_free);
    sem_wait(&cond);

    std::cout << "--- POMBO LEVANDO AS " << cards_counter << " CARTAS ---\n\n";
    cards_counter = 0;
    sleep(2);

    std::cout << "--- POMBO RETORNOU ---\n\n";
    for (int i = 0; i < CARD_LIMIT; ++i) sem_post(&fill_bag);

    sem_post(&cond);
  }

  pthread_exit(0);
}

void create_users()
{
  pthread_t users[USERS], pigeon_t;

  // start pigeon thread
  pthread_create(&pigeon_t, NULL, pigeon, NULL);

  // start users threads
  for (int i = 0; i < CARD_LIMIT; ++i)
  {
    int *id = (int *) malloc(sizeof(int));
    *id = i;

    if (pthread_create(&users[i], NULL, user, (void *) (id)))
    {
      std::cout << "Erro na criação do usuário " << i << std::endl;
      exit(1);
    }
  }

  pthread_join(pigeon_t, NULL);
}

int main()
{
  // Inicia semáforo
  //  Second var refers to pshared
  //  Third value refers to initial value
  sem_init(&cond, 0, 1);
  sem_init(&pigeon_free, 0, 0);
  sem_init(&fill_bag, 0, CARD_LIMIT);
  create_users();

  return 0;
}
