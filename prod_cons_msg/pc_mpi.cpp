// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <mpi.h>

#define TAG 1


int main(int argc, char *argv[]) {
  // initialize MPI and get own id (rank)
  MPI::Init(argc, argv);

  int rank = MPI::COMM_WORLD.Get_rank();
  std::cout << "Sou o processo de número " << rank << std::endl;

  // producer
  if (rank) {
    MPI_Status status;
    char data[5];
    int item;

    while (true) {
      std::cout << "Produtor " << rank << " vai produzir itens..." << std::endl;

      int item = rand() % 100; // something to put on buffer

      sleep(3);

      std::cout << "Produtor produziu " << item << " itens." << std::endl;

      // wait for an empty to arrive
      MPI_Recv(data, 5, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      std::cout << "Produtor recebeu a mensagem " << data << std::endl;

      // send items to consumer
      MPI_Send(&item, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);

      std::cout << "Produtor enviou " << item << " itens para o consumidor."
                << std::endl;
    }
  // consumer
  } else {
    char data[5] = "WANT";
    MPI_Status status;
    int item;

    std::cout << "Consumidor quer consumir..." << std::endl;

    // send 1 empties
    MPI_Send(&data, 5, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);

    while (true) {
      // get message containing item
      MPI_Recv(&item, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      // extract item from message
      std::cout << "Consumidor recebeu " << item << " itens..." << std::endl;

      // do something with the item
      std::cout << "Consumidor consumindo itens..." << std::endl;

      while (item) {
        int item_remove = rand() % (item + 1);
        item -= item_remove;
        std::cout << "Consumidor consome " << item_remove << " itens..."
                  << std::endl;
        sleep(1);
      }

      std::cout << "Consumidor terminou de consumir os itens..." << std::endl;

      // send back empty reply
      MPI_Send(&data, 5, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();
  return 0;
}
