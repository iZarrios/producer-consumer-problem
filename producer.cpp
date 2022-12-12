#include <string>
#define dbg(a) cout << #a << "=" << a << endl

#include "msg.h"
#include <bits/stdc++.h>
#include <chrono>

#include "Queue.h"

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

void signalHandler(int sig_num);

key_t sem_key1;
key_t sem_key2;
key_t sem_key3;
int shmid;
struct shared_mem *q;

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cout << "Error wrong number of args\n";
        return 0;
    }

    /* signal(SIGINT, signalHandler); */

    char *commodity = argv[1];
    double mean = stod(argv[2]);
    double std_dev = stod(argv[3]);
    int sleep = stoi(argv[4]);

    union semun {
        int val;
        struct semid_ds *buf;
        ushort array[1]; // or ushort * array
    } sem_attr;

    int sem_mutex, sem_buffer, sem_signal;

    /* Mutual Exclusion Semaphore */

    if ((sem_key1 = ftok(SEM_MUTEX, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_mutex = semget(sem_key1, 1, PERMS)) == -1)
    {
        cout << "mutex_key semget- error\n";
        return -1;
    }

    key_t key;

    /* SHARED MEMORY */

    if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    if ((shmid = shmget(key, sizeof(struct shared_mem), PERMS)) == -1)
    /* if ((shmid = shmget(key, sizeof(struct shared_mem), PERMS)) == -1) */
    {
        cout << "shmget - error\n";
        return -1;
    }

    printf("Ready to Send\n");

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);

    q = (struct shared_mem *)tmp;

    if (q == (struct shared_mem *)-1)
    {
        printf("Q is -1");
        return -1;
    }

    /* Buffer Semaphore Count */
    if ((sem_key2 = ftok(SEM_BUFFER_COUNT, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_buffer = semget(sem_key2, 1, PERMS)) == -1)
    {
        cout << "sem_buffer semget- error\n";
        return -1;
    }

    /* Signal Semaphore */

    if ((sem_key3 = ftok(SEM_SIG, 'A')) == -1)
    {
        cout << "sem_sig frtok - error\n";
        return -1;
    }
    if ((sem_signal = semget(sem_key3, 1, PERMS)) == -1)
    {
        cout << "sem-sig semget- error\n";
        return -1;
    }

    /* Queue *q = (Queue *)tmp; */

    struct sembuf sem_buf[1];

    sem_buf[0].sem_num = 0;
    sem_buf[0].sem_op = 0;
    sem_buf[0].sem_flg = 0;

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, std_dev);
    double number;
    while (true)
    {
        // check if we can produce
        sem_buf[0].sem_op = -1;
        if (semop(sem_buffer, sem_buf, 1) == -1)
        {
            printf("Producer sem buffer");
            return -1;
        }
        cout<<"I Can produce"<<endl;
        // get mutual exclusion(lock mutex)
        sem_buf[0].sem_op = -1;

        if (semop(sem_mutex, sem_buf, 1) == -1)
        {
            printf("Producer mutex");
            return -1;
        }
        cout<<"Got Mutual Exclusion"<<endl;

        // critical section here

        struct mymsg_buffer to_push = q->data[q->buffer_index];

        number = distribution(generator);

        q->data[q->buffer_index].price = number;

        strcpy(q->data[q->buffer_index].name, commodity);


        q->buffer_index++;

        if (q->buffer_index == SHARED_MEM_SIZE)
        {
            q->buffer_index = 0;
        }

        // end of critical section

        // release mutex
        sem_buf[0].sem_op = 1;

        if (semop(sem_mutex, sem_buf, 1) == -1)
        {
            printf("Producer mutex");
            return -1;
        }

        sem_buf[0].sem_op = 1;

        if (semop(sem_signal, sem_buf, 1) == -1)
        {
            printf("Producer signal");
            return -1;
        }

        this_thread::sleep_for(chrono::milliseconds(sleep));
    }

    return 0;
}

void delete_semaphore(key_t sem_key)
{
    int sem_id, ctl_res;
    if ((sem_id = semget(sem_key, 1, 0)) == -1)
    {
        printf("error getting key while deleting semaphore");
        return;
    }
    if ((ctl_res = semctl(sem_id, 0, IPC_RMID)) == -1)
    {
        printf("error deleting semaphore");
        return;
    }
}

void signalHandler(int sig_num)
{
    shmctl(shmid, IPC_RMID, nullptr);
    shmdt(q);
    delete_semaphore(sem_key1);
    delete_semaphore(sem_key2);
    delete_semaphore(sem_key3);
    printf("done");
    exit(0);
    signal(SIGINT, signalHandler);
}
