#include <bits/stdc++.h>
#include <unistd.h>

#include <chrono>

#include "Queue.h"
#include "msg.h"

using namespace std;

key_t sem_key1;
key_t sem_key2;
key_t sem_key3;
int shmid;
struct shared_memory *q;

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cout << "Error wrong number of args\n";
    return 0;
  }

  char *commodity = argv[1];
  double mean = stod(argv[2]);
  double std_dev = stod(argv[3]);
  int sleep = stoi(argv[4]);
  int buffer_size = atoi(argv[5]);

  union semun {
    int val;
    struct semid_ds *buf;
    ushort array[1];  // or ushort * array
  } sem_attr;

  int sem_mutex, sem_buffer, sem_server;

  /* Mutual Exclusion Semaphore */

  if ((sem_key1 = ftok(SEM_MUTEX, 'A')) == -1) {
    cout << "mutex_key frtok - error\n";
    return -1;
  }
  if ((sem_mutex = semget(sem_key1, 1, PERMS)) == -1) {
    cout << "mutex_key semget- error\n";
    return -1;
  }

  key_t key;

  /* SHARED MEMORY */

  if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1) {
    cout << "frtok - error\n";
    return -1;
  }

  if ((shmid = shmget(key, sizeof(struct shared_memory), PERMS)) == -1)
  /* if ((shmid = shmget(key, sizeof(struct shared_mem), PERMS)) == -1) */
  {
    cout << "shmget - error\n";
    return -1;
  }

  printf("Ready to Send\n");

  // attach to shared memory
  void *tmp = shmat(shmid, (void *)0, 0);

  // cast to our shared memory struct
  q = (struct shared_memory *)tmp;

  // if pointer is null
  if (q == (struct shared_memory *)-1) {
    printf("Q is -1");
    return -1;
  }

  /* Buffer Semaphore Count */
  if ((sem_key2 = ftok(SEM_BUFFER_COUNT, 'A')) == -1) {
    cout << "mutex_key frtok - error\n";
    return -1;
  }
  if ((sem_buffer = semget(sem_key2, 1, PERMS)) == -1) {
    cout << "sem_buffer semget- error\n";
    return -1;
  }

  /* Signal Semaphore for reading */
  if ((sem_key3 = ftok(SEM_SERVER, 'A')) == -1) {
    cout << "sem_sig frtok - error\n";
    return -1;
  }
  if ((sem_server = semget(sem_key3, 1, PERMS)) == -1) {
    cout << "sem-sig semget- error\n";
    return -1;
  }

  /* Queue *q = (Queue *)tmp; */

  struct sembuf sem_buf[1];

  sem_buf[0].sem_num = 0;
  sem_buf[0].sem_op = 0;
  sem_buf[0].sem_flg = 0;

  struct tm tm_now;
  time_t now;

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(mean, std_dev);

  char custom_message[1024];
  double number;
  while (true) {
    // check if we can produce
    sem_buf[0].sem_op = -1;
    if (semop(sem_buffer, sem_buf, 1) == -1) {
      printf("Producer sem buffer\n");
      return -1;
    }

    // yes.
    now = time(nullptr);

    sem_buf[0].sem_op = -1;
    if (semop(sem_mutex, sem_buf, 1) == -1) {
      printf("Producer sem buffer\n");
      return -1;
    }

    // critical section here

    struct mymsg_buffer to_push = q->data[q->buffer_index_produce];

    number = distribution(generator);

    sprintf(custom_message, "generating a new value %.2lf", number);

    my_log_msg(commodity, tm_now, now, custom_message);

    q->data[q->buffer_index_produce].price = number;

    strcpy(q->data[q->buffer_index_produce].name, commodity);

    q->buffer_index_produce++;

    sprintf(custom_message, "placing %.2lf on shared buffer", number);

    my_log_msg(commodity, tm_now, now, custom_message);

    if (q->buffer_index_produce == q->N) {
      q->buffer_index_produce = 0;
    }

    // end of critical section

    // release mutex
    sem_buf[0].sem_op = 1;

    if (semop(sem_mutex, sem_buf, 1) == -1) {
      printf("Producer mutex");
      return -1;
    }

    sem_buf[0].sem_op = 1;

    if (semop(sem_server, sem_buf, 1) == -1) {
      printf("Producer server sem");
      return -1;
    }

    sprintf(custom_message, "sleeping for %d ms", sleep);
    my_log_msg(commodity, tm_now, now, custom_message);

    this_thread::sleep_for(chrono::milliseconds(sleep));
  }

  return 0;
}
