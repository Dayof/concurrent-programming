// Aluna > Dayanne Fernandes da Cunha
// Matrícula > 130107191

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>

#define M 10

pthread_mutex_t check_crossing = PTHREAD_MUTEX_INITIALIZER;

pthread_t all[M];

// monkey info
std::vector<int> request_to_cross(M);
std::map<int, bool> crossing_info;
std::vector<int> monkeys_dir(M);
int who_is_crossing = -1;

bool check_no_one_is_crossing()
{
  for (auto& i : crossing_info) if (i.second == true) return false;
  return true;
}

void *cross_string (void *arg)
{
  int i = *((int *) arg);

  while (1)
  {
    sleep(1);
    std::cout << "--- MONKEY " << i << " RESQUEST TO CROSS FROM " << monkeys_dir[i]
                                                                  << " ---\n\n";
    // monkey i request to cross the string
    pthread_mutex_lock(&check_crossing);
    request_to_cross[i]++;
    if (request_to_cross[i] == 1)
    {
      // if no monkey is crossing, them cross him
      if (who_is_crossing == -1)
      {
        // monkey i cross the string
        who_is_crossing = i;
        crossing_info[who_is_crossing] = true;
        std::cout << "--- MONKEY " << i << " IS CROSSING FROM "
                  << monkeys_dir[who_is_crossing] << " --- \n\n";
      }
      // check directions
      else if (monkeys_dir[who_is_crossing] == monkeys_dir[i])
      {
        std::cout << "--- ANOTHER MONKEY " << i << " RESQUEST TO CROSS FROM "
                << monkeys_dir[i] << " FOLLOWING MONKEY " << who_is_crossing
                << " THROUGHT DIRECTION " << monkeys_dir[who_is_crossing] << " ---\n\n";
        // monkey i cross the string
        who_is_crossing = i;
        crossing_info[who_is_crossing] = true;
        std::cout << "--- MONKEY " << i << " IS CROSSING FROM "
                  << monkeys_dir[who_is_crossing] << " --- \n\n";
      }
      else
      {
        std::cout << "--- MONKEY " << i << " REJECT TO CROSS BECAUSE HE IS FROM "
                    << monkeys_dir[i] << " AND THE MONKEY " << who_is_crossing
                    << " IS COMING FROM " << monkeys_dir[who_is_crossing] << " ---\n\n";
        request_to_cross[i] = 0;
      }
    }
    pthread_mutex_unlock(&check_crossing);

    // monkey passing throught the string
    sleep(1);

    pthread_mutex_lock(&check_crossing);
    // to prevent enter in this lock first
    request_to_cross[i]--;
    if (request_to_cross[i] == 0)
    {
      monkeys_dir[i] = !monkeys_dir[i];
      crossing_info[who_is_crossing] = false;
      std::cout << "--- MONKEY " << i << " CROSSED TO " << monkeys_dir[i] << " --- \n\n";
      if (check_no_one_is_crossing) who_is_crossing = -1;
    }
    pthread_mutex_unlock(&check_crossing);
  }

  pthread_exit(0);
}

void create_monkeys()
{
  for (int i = 0; i < M ; i++) {
      int *id = (int *) malloc(sizeof(int));
      *id = i;
      // left border
      if (i <= 4) monkeys_dir[i] = 0;
      // rigth border
      else monkeys_dir[i] = 1;
      crossing_info[i] = false;
      request_to_cross[i] = 0;
      pthread_create(&all[i], NULL, cross_string, (void *) (id));
  }
}

int main()
{
  // create readers and writters
  create_monkeys();

  pthread_join(all[0], NULL);

  return 0;
}
