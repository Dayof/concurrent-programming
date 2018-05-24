// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>

#define BRANCHS_SIZE 10
#define M_SIZE 10

pthread_barrier_t barrier;

void *branch(void *id);

std::vector<std::vector<int> > matrix(M_SIZE, std::vector<int>(M_SIZE, 5));
std::vector<int> result(BRANCHS_SIZE, 0);


void *branch(void *id) {
  int pid = *(int *)(id);

  std::cout << "Branch " << pid << " começou." << std::endl;

  for (int j = 0; j < M_SIZE; ++j) result[pid] += matrix[pid][j];

  pthread_barrier_wait(&barrier);

  if (pid == 0) {
    float total_sum = 0;
    for (int i = 0; i < BRANCHS_SIZE; i++) total_sum += result[i];
    std::cout << "Total sum of the matrix is :" << total_sum << std::endl;
  }

  pthread_exit(0);
}

void create_barriers() {
  pthread_t branchs[BRANCHS_SIZE];
  int *id;

  pthread_barrier_init(&barrier, NULL, BRANCHS_SIZE);

  for (int i = 0; i < BRANCHS_SIZE; ++i) {
    id = (int *) malloc(sizeof(int));
    *id = i;

    if (pthread_create(&branchs[i], NULL, branch, (void *) (id))) {
      std::cout << "Erro na criacao da branch de soma " << i << std::endl;
      exit(1);
    }
  }

  for (int i = 0; i < BRANCHS_SIZE; ++i)
     if (pthread_join(branchs[i], NULL)) {
        std::cout << "Erro no 'joining' das branchs."<< std::endl;
        exit(1);
     }
}

int main() {
  create_barriers();

  return 0;
}
