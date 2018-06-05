// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191
//
// Trabalho final de Programação Concorrente
// Problema : Software de controle de robótica cirúrgica

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <queue>
#include <map>

#define BUFFER_COMMANDS 3
#define NUM_MOTORS 3

#define COMMAND_X 'X'
#define COMMAND_Y 'Y'
#define COMMAND_Z 'Z'

void *joystick(void *id);
void *control_connections(void *id);
void *motors_interface(void *id);
void *motor_x(void *id);
void *motor_y(void *id);
void *motor_z(void *id);

void show_buffer(std::queue<std::string> tmp_buffer);
bool check_command_input(std::string command);
void process_command();

sem_t buffer;
sem_t command_buffer;
sem_t command_execution_x;
sem_t command_execution_y;
sem_t command_execution_z;

pthread_mutex_t mutex_command_exec = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_connection = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_command = PTHREAD_MUTEX_INITIALIZER;

std::string current_command, command_to_exec, last_command;
std::queue<std::string> vec_commands;
bool connection_x = false, connection_y = false, connection_z = false;

static bool DEBUG = false;

void show_buffer(std::queue<std::string> tmp_buffer) {
  while (!tmp_buffer.empty()) {
    std::cout << tmp_buffer.front() << " ";
    tmp_buffer.pop();
  }
  std::cout << std::endl;
}

bool check_command_input(std::string command) {
  return command == "X+" || command == "X-" ||
          command == "Y+" || command == "Y-" ||
          command == "Z+" || command == "Z-";
}

void *joystick(void* id) {
  int buffer_counter;

  if (DEBUG) std::cout << "--- Joystick criado ---\n\n";
  std::cout << "------ BEM VINDA(O) ------" << std::endl;
  std::cout << "- BUFFER DO JOYSTICK : " << BUFFER_COMMANDS << std::endl;
  std::cout << "- NÚMERO DE SERVOMOTORES : " << NUM_MOTORS << std::endl;
  while (true) {

    // try receive any commands from joystick
    // continue thread if any command was received
    std::cout << "- INSIRA UM COMANDO DO JOYSTICK" << std::endl;
    std::cout << "Opções: X+, X-, Y+, Y-, Z+, Z-\n> ";
    std::cin >> current_command;

    if (check_command_input(current_command)) {

      // check buffer's size
      sem_getvalue(&buffer, &buffer_counter);
      if (DEBUG) std::cout << "--- Valor do buffer : " << buffer_counter
                            << " --- " << std::endl;
      if (buffer_counter == 0)
        std::cout << "Comando ' " << current_command
                  << " ' será descartado pois o buffer de comandos "
                  << "já está lotado." << std::endl;
      else {
        vec_commands.push(current_command);

        std::cout << "-- COMANDO RECEBIDO, BUFFER ATUAL --" << std::endl;
        std::queue<std::string> tmp_buffer(vec_commands);
        show_buffer(tmp_buffer);

        sem_post(&command_buffer);
      }
      sleep(2);
    } else {
      std::cout << "[ERROR] Comando inválido, tente novamente.\n";
      continue;
    }
  }
  pthread_exit(0);
}

void *motors_interface(void* id) {
  int pid = *(int *)(id);

  if (DEBUG) std::cout << "--- Interface de motores "
                        << pid << " criada ---\n\n";
  while (true) {
    // if buffer is not full then pass the command to motors interface
    // proccess the next steps
    // interfaces are locked until a command is not sent to them
    if (DEBUG) std::cout << "--- Interface " << pid
                          << " aguardando comando ---" << std::endl;
    sem_wait(&command_buffer);
    sem_wait(&buffer);
    std::cout << "--- Interface " << pid
              << " irá processar o comando recebido ---" << std::endl;
    // check connections and check which motor will be used
    process_command();
  }
  pthread_exit(0);
}

void process_command() {
  int value;

  // block other threads check current command, proccess one command per time
  pthread_mutex_lock(&mutex_command);

  if (DEBUG) std::cout << "--- Processando o comando "
                        << current_command << " ---"
                        << std::endl;
  // check which motor will be used
  switch (current_command[0]) {
      case COMMAND_X: {
        std::cout << "--- Comando para o motor X processado ---" << std::endl;
        // block the thread control_connections or motors to modify some motor
        // status of connection
        pthread_mutex_lock(&mutex_connection);

        std::cout << "--- Checando comunicação do motor X ---" << std::endl;
        // if a command X is sent and motor is connected, then sent
        // this command to be executed
        if (connection_x) {
          std::cout << "--- Motor X conectado ---" << std::endl;
          pthread_mutex_unlock(&mutex_connection);
          sem_post(&command_execution_x);
        }
        else std::cout << "--- [ERROR] Motor X está desconectado ---"
                        << std::endl;

        break;
      }
      case COMMAND_Y: {
        // block the thread control_connections or motors to modify some motor
        // status of connection
        pthread_mutex_lock(&mutex_connection);

        std::cout << "--- Checando comunicação do motor Y ---" << std::endl;
        // if a command Y is sent and motor is connected, then sent
        // this command to be executed
        if (connection_y) {
          std::cout << "--- Motor Y conectado ---" << std::endl;
          pthread_mutex_unlock(&mutex_connection);
          sem_post(&command_execution_y);
        }
        else std::cout << "--- [ERROR] Motor Y está desconectado ---"
                        << std::endl;
        break;
      }
      case COMMAND_Z: {
        // block the thread control_connections or motors to modify some motor
        // status of connection
        pthread_mutex_lock(&mutex_connection);

        std::cout << "--- Checando comunicação do motor Z ---" << std::endl;
        // if a command Z is sent and motor is connected, then sent
        // this command to be executed
        if (connection_z) {
          std::cout << "--- Motor Z conectado ---" << std::endl;
          pthread_mutex_unlock(&mutex_connection);
          sem_post(&command_execution_z);
        }
        else std::cout << "--- [ERROR] Motor Z está desconectado ---"
                        << std::endl;
        break;
      }
      // handles any other missing/unmapped cases
      default: {
          std::cout << "[ERROR] Comando não reconhecido pela interface."
                    << std::endl;
          break;
      }
  }
  pthread_mutex_unlock(&mutex_command);
}

void *control_connections(void* id) {
  while (true) {
    // block the thread 'process_command' or 'motors' to modify some motor
    // status of connection
    pthread_mutex_lock(&mutex_connection);

    if (DEBUG) std::cout << "--- CONTROLANDO AS CONEXÕES --- " << std::endl;
    // set all motors connections to false to wait the motors itselves
    // say their correct status instead
    connection_x = false; connection_y = false; connection_z = false;
    // every half a minute this control try to change the motors status
    pthread_mutex_unlock(&mutex_connection);
    sleep(30);

  }
  pthread_exit(0);
}

void *motor_x(void *id) {
  if (DEBUG) std::cout << "--- Motor X criado ---\n\n";
  while (true) {
    // std::cout << "--- Motor X pronto para executar comando ---" << std::endl;
    // wait until a signal to execute a command on servomotor x arrives
    if (sem_trywait(&command_execution_x) == 0) {


      std::cout << "--- COMANDO X RECEBIDO ---" << std::endl;
      if (!last_command.empty()) std::cout << "--- AGUARDANDO COMANDO "
                                            << last_command
                                            << " SER EXECUTADO ---"
                                            << std::endl;
      last_command = vec_commands.front();
      // block other motors execute at the same time
      pthread_mutex_lock(&mutex_command_exec);

      std::cout << "--- EXECUTANDO COMANDO " << vec_commands.front()
                << " ---" << std::endl;

      // command being executed
      sleep(10);

      std::cout << "--- COMANDO NO MOTOR X EXECUTADO ---" << std::endl;
      std::cout << "--- Liberando espaço no buffer ---" << std::endl;
      sem_post(&buffer);

      // remove command processed from the queue of commands
      vec_commands.pop();

      pthread_mutex_unlock(&mutex_command_exec);

      last_command.clear();
    // if there isn't possible that 'motor_x' execute a command then
    // at least warns the system that this motor is alive
    } else {
      if (DEBUG) std::cout << "--- Aguardando comando no motor X ---"
                            << std::endl;
      // block the thread 'process_command' or 'control_connections' to
      // modify some motor status of their connection
      pthread_mutex_lock(&mutex_connection);
      if (DEBUG) std::cout << "--- Motor X ativo ---" << std::endl;
      connection_x = true;
      pthread_mutex_unlock(&mutex_connection);
      sleep(2);
    }
  }
  pthread_exit(0);
}

void *motor_y(void *id) {
  if (DEBUG) std::cout << "--- Motor Y criado ---\n\n";
  while (true) {
    if (DEBUG) std::cout << "--- Motor Y pronto para executar comando ---"
                          << std::endl;
    // wait until a signal to execute a command on servomotor y arrives
    if (sem_trywait(&command_execution_y) == 0) {

      std::cout << "--- COMANDO Y RECEBIDO ---" << std::endl;
      if (!last_command.empty()) std::cout << "--- AGUARDANDO COMANDO "
                                            << last_command
                                            << " SER EXECUTADO ---"
                                            << std::endl;

      last_command = vec_commands.front();
      // block other motors execute at the same time
      pthread_mutex_lock(&mutex_command_exec);

      std::cout << "--- EXECUTANDO COMANDO " << vec_commands.front()
                << " ---" << std::endl;

      // command being executed
      sleep(10);

      std::cout << "--- COMANDO NO MOTOR Y EXECUTADO ---" << std::endl;
      std::cout << "--- Liberando espaço no buffer ---" << std::endl;
      sem_post(&buffer);

      // remove command processed from the queue of commands
      vec_commands.pop();

      pthread_mutex_unlock(&mutex_command_exec);

      last_command.clear();
    // if there isn't possible that 'motor_y' execute a command then
    // at least warns the system that this motor is alive
    } else {
      if (DEBUG) std::cout << "--- Aguardando comando no motor Y ---"
                            << std::endl;
      // block the thread 'process_command' or 'control_connections' to
      // modify some motor status of their connection
      pthread_mutex_lock(&mutex_connection);
      if (DEBUG) std::cout << "--- Motor Y ativo ---" << std::endl;
      connection_y = true;
      pthread_mutex_unlock(&mutex_connection);
      sleep(2);
    }
  }
  pthread_exit(0);
}

void *motor_z(void *id) {
  if (DEBUG) std::cout << "--- Motor Z criado ---\n\n";
  while (true) {
    if (DEBUG) std::cout << "--- Motor Z pronto para executar comando ---"
                          << std::endl;
    // wait until a signal to execute a command on servomotor z arrives
    if (sem_trywait(&command_execution_z) == 0) {

      std::cout << "--- COMANDO Z RECEBIDO ---" << std::endl;
      if (!last_command.empty()) std::cout << "--- AGUARDANDO COMANDO "
                                            << last_command
                                            << " SER EXECUTADO ---"
                                            << std::endl;
      last_command = vec_commands.front();
      // block other motors execute at the same time
      pthread_mutex_lock(&mutex_command_exec);

      std::cout << "--- EXECUTANDO COMANDO " << vec_commands.front()
                << " ---" << std::endl;

      // command being executed
      sleep(10);

      std::cout << "--- COMANDO NO MOTOR Z EXECUTADO ---" << std::endl;
      std::cout << "--- Liberando espaço no buffer ---" << std::endl;
      sem_post(&buffer);

      // remove command processed from the queue of commands
      vec_commands.pop();

      pthread_mutex_unlock(&mutex_command_exec);

      last_command.clear();
    // if there isn't possible that 'motor_z' execute a command then
    // at least warns the system that this motor is alive
    } else {
      if (DEBUG) std::cout << "--- Aguardando comando no motor Z ---"
                            << std::endl;
      // block the thread 'process_command' or 'control_connections' to
      // modify some motor status of their connection
      pthread_mutex_lock(&mutex_connection);
      if (DEBUG) std::cout << "--- Motor Z ativo ---" << std::endl;
      connection_z = true;
      pthread_mutex_unlock(&mutex_connection);
      sleep(2);
    }
  }
  pthread_exit(0);
}

void create_threads() {
  pthread_t t_joystick, t_control_connections,
            t_motors_interface[BUFFER_COMMANDS], t_motors[NUM_MOTORS];

  // starts joytick thread
  pthread_create(&t_joystick, NULL, joystick, NULL);

  // starts control motors connections thread
  pthread_create(&t_control_connections, NULL, control_connections, NULL);

  // starts motors threads
  pthread_create(&t_motors[0], NULL, motor_x, NULL);
  pthread_create(&t_motors[1], NULL, motor_y, NULL);
  pthread_create(&t_motors[2], NULL, motor_z, NULL);

  // starts control motors interfaces thread
  for (int i = 0; i < BUFFER_COMMANDS; ++i) {
    int *id = (int *) malloc(sizeof(int));
    *id = i;

    if (pthread_create(&t_motors_interface[i], NULL,
                      motors_interface, (void *) (id))) {
      std::cout << "Erro na criação da interface de motor " << i << std::endl;
      exit(1);
    }
  }

  if (DEBUG) std::cout << "--- THREADS CRIADAS ---" << std::endl;

  // wait until joystick thread finish
  pthread_join(t_joystick, NULL);
}

// main function, starts here
int main() {
  sem_init(&command_buffer, 0, 0);
  sem_init(&buffer, 0, BUFFER_COMMANDS);
  sem_init(&command_execution_x, 0, 0);
  sem_init(&command_execution_y, 0, 0);
  sem_init(&command_execution_z, 0, 0);

  if (DEBUG) std::cout << "--- INÍCIO DA CRIAÇÃO DAS THREADS ---" << std::endl;
  create_threads();

  return 0;
}
