/*
 *
 *       spooler.c: Print strings in the shared memory segment
 *                  (Consumer process)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <bits/stdc++.h>

// Buffer data structures
#define MAX_BUFFERS 10
#define SHARED_MEMORY_KEY "shared_memory_key"
#define SEM_MUTEX_KEY "sem-mutex-key"
#define SEM_BUFFER_COUNT_KEY "sem-buffer-count-key"
#define SEM_SPOOL_SIGNAL_KEY "sem-spool-signal-key"
#define PROJECT_ID 'K'

using namespace std;

struct shared_memory
{
    char buf[MAX_BUFFERS][256];
    int buffer_index;
    int buffer_print_index;
};

void error(string msg);

int main(int argc, char **argv)
{
    key_t s_key;
    union semun {
        int val;
        struct semid_ds *buf;
        ushort array[1];
    } sem_attr;
    int shm_id;
    struct shared_memory *shared_mem_ptr;
    int mutex_sem, buffer_count_sem, spool_signal_sem;

    printf("spooler: hello world\n");
    //  mutual exclusion semaphore
    /* generate a key for creating semaphore  */
    if ((s_key = ftok(SEM_MUTEX_KEY, PROJECT_ID)) == -1)
        error("ftok");
    if ((mutex_sem = semget(s_key, 1, 0660 | IPC_CREAT)) == -1)
        error("semget");
    // Giving initial value.
    sem_attr.val = 0; // locked, till we finish initialization
    if (semctl(mutex_sem, 0, SETVAL, sem_attr) == -1)
        error("semctl SETVAL");

    // Get shared memory
    if ((s_key = ftok(SHARED_MEMORY_KEY, PROJECT_ID)) == -1)
        error("ftok");
    if ((shm_id = shmget(s_key, sizeof(struct shared_memory), 0660 | IPC_CREAT)) == -1)
        error("shmget");
    if ((shared_mem_ptr = (struct shared_memory *)shmat(shm_id, NULL, 0)) == (struct shared_memory *)-1)
        error("shmat");
    // Initialize the shared memory
    shared_mem_ptr->buffer_index = shared_mem_ptr->buffer_print_index = 0;

    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok(SEM_BUFFER_COUNT_KEY, PROJECT_ID)) == -1)
        error("ftok");
    if ((buffer_count_sem = semget(s_key, 1, 0660 | IPC_CREAT)) == -1)
        error("semget");
    // giving initial values
    sem_attr.val = MAX_BUFFERS; // MAX_BUFFERS are available
    if (semctl(buffer_count_sem, 0, SETVAL, sem_attr) == -1)
        error("semctl SETVAL");

    // counting semaphore, indicating the number of strings to be printed.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok(SEM_SPOOL_SIGNAL_KEY, PROJECT_ID)) == -1)
        error("ftok");
    if ((spool_signal_sem = semget(s_key, 1, 0660 | IPC_CREAT)) == -1)
        error("semget");
    // giving initial values
    sem_attr.val = 0; // 0 strings are available initially.
    if (semctl(spool_signal_sem, 0, SETVAL, sem_attr) == -1)
        error("semctl SETVAL");

    // Initialization complete; now we can set mutex semaphore as 1 to
    // indicate shared memory segment is available
    sem_attr.val = 1;
    if (semctl(mutex_sem, 0, SETVAL, sem_attr) == -1)
        error("semctl SETVAL");

    struct sembuf asem[1];

    asem[0].sem_num = 0;
    asem[0].sem_op = 0;
    asem[0].sem_flg = 0;

    while (1)
    { // forever
        // Is there a string to print? P (spool_signal_sem);
        asem[0].sem_op = -1;
        if (semop(spool_signal_sem, asem, 1) == -1)
            perror("semop: spool_signal_sem");

        printf("%s", shared_mem_ptr->buf[shared_mem_ptr->buffer_print_index]);

        /* Since there is only one process (the spooler) using the
           buffer_print_index, mutex semaphore is not necessary */
        (shared_mem_ptr->buffer_print_index)++;
        if (shared_mem_ptr->buffer_print_index == MAX_BUFFERS)
            shared_mem_ptr->buffer_print_index = 0;

        /* Contents of one buffer has been printed.
           One more buffer is available for use by producers.
           Release buffer: V (buffer_count_sem);  */
        asem[0].sem_op = 1;
        if (semop(buffer_count_sem, asem, 1) == -1)
            perror("semop: buffer_count_sem");
    }
}

// Print system error and exit
void error(string msg)
{
    perror(msg.c_str());
    exit(1);
}
