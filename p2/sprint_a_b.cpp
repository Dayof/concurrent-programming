// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <semaphore.h>

#define QTD_A_B 10

sem_t print_A;
sem_t print_AC;
sem_t print_B;
sem_t print_BC;


void *show_A_B(void *id) {
  int pid = *(int *)(id), value;

	while (true) {
    std::cout << "A";
    if (sem_trywait(&print_A) == -1) {
      for (int j=0; j < QTD_A_B-1; ++j) {
        sem_post(&print_A);
        sem_post(&print_AC);
      }
    } else sem_wait(&print_AC);

    std::cout << "B";
    if (sem_trywait(&print_B) == -1) {
      std::cout << std::endl;
      sleep(1);
      for (int j=0; j < QTD_A_B-1; ++j) {
        sem_post(&print_B);
        sem_post(&print_BC);
      }
    } else sem_wait(&print_BC);
	}
}

void create_threads() {
  pthread_t branchs[QTD_A_B];
  int *id;

  for (int i = 0; i < QTD_A_B; ++i) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    if (pthread_create(&branchs[i], NULL, show_A_B, (void *) (id))) {
      std::cout << "Erro na criacao da branch " << i << std::endl;
      exit(1);
    }
  }

  pthread_join(branchs[0],NULL);
}

int main() {
  sem_init(&print_A, 0, QTD_A_B-1);
  sem_init(&print_AC, 0, 0);
  sem_init(&print_B, 0, QTD_A_B-1);
  sem_init(&print_BC, 0, 0);
  create_threads();

	return 0;
}
