#include <string>
#define dbg(a) cout << #a << "=" << a << endl
#define PERMS 0644

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

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cout << "Error wrong number of args\n";
        return 0;
    }
    char *commodity = argv[1];
    double mean = stod(argv[2]);
    double std_dev = stod(argv[3]);
    int sleep = stoi(argv[4]);

    key_t sem_key;

    int sem_mutex, sem_buffer, sem_signal;

    /* Mutual Exclusion Semaphore */

    if ((sem_key = ftok(SEM_MUTEX, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_mutex = semget(sem_key, 1, PERMS)) == -1)
    {
        cout << "mutex_key semget- error\n";
        return -1;
    }

    /* Buffer Semaphore Count */
    if ((sem_key = ftok(SEM_BUFFER_COUNT, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_buffer = semget(sem_key, 1, PERMS)) == -1)
    {
        cout << "sem_buffer semget- error\n";
        return -1;
    }

    /* Signal Semaphore */

    if ((sem_key = ftok(SEM_SIG, 'A')) == -1)
    {
        cout << "sem_sig frtok - error\n";
        return -1;
    }
    if ((sem_signal = semget(sem_key, 1, PERMS)) == -1)
    {
        cout << "sem-sig semget- error\n";
        return -1;
    }

    int shmid;

    key_t key;

    /* SHARED MEMORY */

    if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    if ((shmid = shmget(key, sizeof(struct mymsg_buffer) * SHARED_MEM_SIZE, PERMS)) == -1)
    {
        cout << "shmget - error\n";
        return -1;
    }

    int indexer_shmid;
    key_t indexer_key;

    /* Indexer SHARED MEMORY */
    if ((indexer_key = ftok(INDEXER_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    if ((indexer_shmid = shmget(indexer_key, sizeof(int) * 2, PERMS)) == -1)
    {
        cout << "smsget - error\n";
        return -1;
    }

    printf("Ready to Send\n");

    int *indexer_shm;
    struct mymsg_buffer *q;

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);
    void *tmp1 = shmat(indexer_shmid, (void *)0, 0);

    q = (struct mymsg_buffer *)tmp;

    indexer_shm = (int *)tmp1;

    cout << *indexer_shm << endl;
    *indexer_shm += 1;

    /* Queue *q = (Queue *)tmp; */

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, std_dev);
    while (true)
    {
        double number = distribution(generator);
        if (*indexer_shm == SHARED_MEM_SIZE) // 1024
        {
            *indexer_shm = 1;
        }

        struct mymsg_buffer to_push;
        to_push.price = number;
        strcpy(to_push.name, commodity);

        q[*indexer_shm] = to_push;

        *indexer_shm += 1;

        cout << "Increased Indexer=" << *indexer_shm << endl;

        /* q->push(to_push); */

        /* printf("Size of Queue: %lld\n", q->getSize()); */

        this_thread::sleep_for(chrono::milliseconds(sleep));
    }

    shmdt(q);
    return 0;
}
