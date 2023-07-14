#pragma once

#define SHARED_MEM_NAME "../shared_mem"

#define SHARED_MEM_SIZE 100

#define SEM_MUTEX "../sem-mutex"
#define SEM_BUFFER_COUNT "../sem-buffer-client"
#define SEM_SERVER "../sem-server"

#define UP_ARROW "↑"
#define DOWN_ARROW "↓"

#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

#define PERMS 0666

#define UNCHANGED 0
#define INCREASED 1
#define DECREASED -1

#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>

void my_log_msg(char *, struct tm, time_t, std::string);

struct mymsg_buffer {
  char name[11];  // given if max commodity name cant be more than 10 characters
  double price;

  mymsg_buffer() { memset(name, '\0', strlen(name)); }
};

struct shared_memory {
  // same as in the queue implementation we can also reallocate more space if
  // needed by the user
  struct mymsg_buffer data[SHARED_MEM_SIZE + 1];
  int N;  // actual size of the buffer
  int buffer_index_produce;
  int buffer_index_consume;
};

struct shared_memory1 {
  // same as in the queue implementation we can also reallocate more space if
  // needed by the user
  int *data;
  int N;  // actual size of the buffer
  int buffer_index_produce;
  int buffer_index_consume;
};

class SharedMemory {
 public:
  struct mymsg_buffer *data;
  int N;
  int buffer_index_produce;
  int buffer_index_consume;
  SharedMemory(int);
};

class Commodity {
 public:
  std::string name;

  double price;

  double avg_price;

  // we required last AVG_NO(Look msg.h) prices in the problem statement
  // so we are going to store them in a queue
  // and just keep popping and pushing to calculate the average.
  std::queue<double> last_prices;

  // needed to know whether the commodity price  has increased or decreased
  // with respect to the last reading known by our program.
  double last_price;
  double last_avg_price;

  // {UNCHANGED, INCREASED, DECREASED }
  int price_state;
  int avg_price_state;

  Commodity();
  Commodity(std::string, double);
};
