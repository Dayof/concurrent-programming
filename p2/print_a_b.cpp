// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#define QTD_A_B 10

pthread_barrier_t barrier;


void *show_A_B(void *id) {
  int pid = *(int *)(id);

	while (true) {
		std::cout << "A";
		pthread_barrier_wait(&barrier); // wait print A

		std::cout << "B";
		pthread_barrier_wait(&barrier); // wait print C

    int t = rand()%10;
    // std::cout << t << std::endl;
    sleep(t);
    if (pid == QTD_A_B-1) std::cout << std::endl;
    pthread_barrier_wait(&barrier);
	}
}

void create_barriers() {
  pthread_t branchs[QTD_A_B];
  int *id;

  pthread_barrier_init(&barrier, NULL, QTD_A_B);

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
  create_barriers();

	return 0;
}
