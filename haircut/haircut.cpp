// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <semaphore.h>

#define CLIENTS 10
#define CHAIRS_LIMIT 5

void * client(void *id);
void * haircutter(void *id);

sem_t wait_cutting;
sem_t has_client;
sem_t chair_free;
sem_t cutting;

int current_client, clients_served = 0;

void * client(void* id)
{
  int pid = *(int *)(id), chairs;

  std::cout << "--- CLIENTE " << pid << " ENTROU NA LOJA ---\n\n";

  sem_getvalue(&chair_free, &chairs);
  std::cout << "--- CLIENTE " << pid << " OLHOU PARA LOJA E VIU QUE " << chairs
            << " CADEIRAS ESTÃO LIVRES ---\n\n";

  if (sem_trywait(&chair_free) == 0)
  {
    std::cout << "--- CLIENTE " << pid << " ESTÁ ESPERANDO NA CADEIRA ---\n\n";

    sem_getvalue(&chair_free, &chairs);
    std::cout << "--- " << chairs << " CADEIRAS ESTÃO LIVRES ---\n\n";

    std::cout << "--- CLIENTE " << pid << " AGUARDANDO ATENDIMENTO ---\n\n";
    sem_wait(&wait_cutting);

    current_client = pid;
    sem_post(&has_client);

    sem_post(&chair_free);
    std::cout << "--- CLIENTE " << pid
              << " FOI SER ATENTIDO E LIBEROU A CADEIRA ---\n\n";

    sem_getvalue(&chair_free, &chairs);
    std::cout << "--- " << chairs << " CADEIRAS ESTÃO LIVRES ---\n\n";

    sem_wait(&cutting);

    std::cout << "--- CLIENTE " << pid << " FOI EMBORA ---\n\n";

    sem_post(&wait_cutting);
  }
  else
  {
    sem_getvalue(&chair_free, &chairs);
    std::cout << "--- LOJA ESTÁ CHEIA, TODAS AS "
              << CHAIRS_LIMIT << " CADEIRAS ESTÃO OCUPADAS ---\n\n";
    std::cout << "--- CLIENTE " << pid << " SAIU DA LOJA ---\n\n";
  }

  pthread_exit(0);
}

void * haircutter(void* id)
{
  while (1)
  {
    std::cout << "--- CABELEIREIRO PREPARADO PARA ATENDER O PRÓXIMO "
              << "CLIENTE ---\n\n";

    sem_wait(&has_client);
    std::cout << "--- CLIENTE " << current_client
              << " FOI CHAMADO PARA SER ATENDIDO ---\n\n";
    sleep(1);

    std::cout << "--- CLIENTE " << current_client
              << " SENTOU NA CADEIRA DO CABELEIREIRO ---\n\n";
    sleep(1);

    std::cout << "--- CABELEIREIRO CORTANDO CABELO DO CLIENTE "
              << current_client << " ---\n\n";
    sleep(3);

    clients_served++;
    std::cout << "--- CORTE FINALIZADO, CABELEIREIRO SE PREPARA "
              << "PARA CHAMAR PRÓXIMO CLIENTE ---\n\n";
    sem_post(&cutting);
    sleep(1);
  }

  pthread_exit(0);
}

void create_clients_and_haircutter()
{
  pthread_t clients[CLIENTS], haircutter_t;
  int id[CLIENTS];

  // start haircutter thread
  pthread_create(&haircutter_t, NULL, haircutter, NULL);

  sleep(1);

  // start clients threads
  for (int i = 0; i < CLIENTS; ++i)
  {
    id[i] = i;
    if (pthread_create(&clients[i], NULL, client, (void *) &id[i]))
    {
      std::cout << "Erro na criação do cliente " << i << std::endl;
      exit(1);
    }
    sleep(1);
  }

  for (int i = 0; i < CLIENTS; ++i) pthread_join(clients[i], NULL);

  std::cout << "--- O CABELEIREIRO ENCERROU O EXPEDIENTE E CONSEGUIU "
            << "ATENTER " << clients_served << " CLIENTES DE "
            << CLIENTS << " QUE VISITARAM A LOJA ---\n\n";
}

int main()
{
  // Inicia semáforo
  //  Second var refers to pshared
  //  Third value refers to initial value
  sem_init(&chair_free, 0, CHAIRS_LIMIT);
  sem_init(&wait_cutting, 0, 1);
  sem_init(&has_client, 0, 0);
  sem_init(&cutting, 0, 0);

  create_clients_and_haircutter();

  return 0;
}
