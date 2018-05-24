// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#define PHILOSOPHERS 5
#define FORKS 5

void *eat(void *id);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthead_barrier_t
sem_t philosopher_eating;
sem_t fork_free;

void *eat(void* id)
{
  int pid = *(int *)(id);

  while (1)
  {
    pthread_mutex_lock(&mutex);

    std::cout << "--- FILOSOFO " << pid << " ESTÁ REFLETINDO ---\n\n";

    // sem_wait(&pigeon_free);
    // sem_wait(&cond);

    // std::cout << "--- POMBO LEVANDO AS " << cards_counter << " CARTAS ---\n\n";
    // cards_counter = 0;

    sleep(1);

    std::cout << "--- FILOSOFO " << pid << " ESTÁ COMENDO ---\n\n";

    // std::cout << "--- POMBO RETORNOU ---\n\n";
    // for (int i = 0; i < CARD_LIMIT; ++i) sem_post(&fill_bag);

    // sem_post(&cond);
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(0);
}

void create_users()
{
  pthread_t phil_t[PHILOSOPHERS];
  int id[PHILOSOPHERS];

  // start philosopher threads
  for (int i = 0; i < PHILOSOPHERS; ++i)
  {
    id[i] = i;
    if (pthread_create(&phil_t[i], NULL, eat, (void *) &id[i]))
    {
      std::cout << "Erro na criação do filósofo " << i << std::endl;
      exit(1);
    }
  }

  pthread_join(phil_t[0], NULL);
}

int main()
{
	pthread_t blabka;
	pthead_barrier_init(&barreira, NULL, QTD);
	pthead_barrier_wait(&barreira);
  sem_init(&fork_free, 0, FORKS);
  create_users();

  return 0;
}
