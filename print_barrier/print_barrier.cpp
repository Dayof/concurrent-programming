// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#define QTD_A_C 10
#define QTD_B_D 5

pthread_barrier_t barrier;


void *show_A_C(void *id) {
  int pid = *(int *)(id);

	while (true) {
		std::cout << "A";
		pthread_barrier_wait(&barrier); // wait print A
		pthread_barrier_wait(&barrier); // wait print B

		std::cout << "C";
		pthread_barrier_wait(&barrier); // wait print C
    pthread_barrier_wait(&barrier); // wait print D

    if (pid == 9) std::cout << std::endl;
    pthread_barrier_wait(&barrier); // wait print newline
	}
}

void *show_B_D(void *id) {
  int pid = *(int *)(id);

	while (true) {
		pthread_barrier_wait(&barrier); // wait print A

		std::cout << "B";
		pthread_barrier_wait(&barrier); // wait print B
		pthread_barrier_wait(&barrier); // wait print C

		std::cout << "D";
    pthread_barrier_wait(&barrier); // wait print D
    pthread_barrier_wait(&barrier); // wait print newline
	}
}

void create_barriers() {
  pthread_t branchs[QTD_A_C+QTD_B_D];
  int *id;

  pthread_barrier_init(&barrier, NULL, QTD_A_C+QTD_B_D);

  for (int i = 0; i < QTD_A_C; ++i) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    if (pthread_create(&branchs[i], NULL, show_A_C, (void *) (id))) {
      std::cout << "Erro na criacao da branch " << i << std::endl;
      exit(1);
    }

    if (i < 5) {
      *id = QTD_A_C+i;
      if (pthread_create(&branchs[i], NULL, show_B_D, (void *) (id))) {
        std::cout << "Erro na criacao da branch " << i << std::endl;
        exit(1);
      }
    }
  }

  pthread_join(branchs[0],NULL);
}

int main() {
  create_barriers();

	return 0;
}
