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

    int shmid;

    key_t key;

    // get unique key
    if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    if ((shmid = shmget(key, sizeof(struct mymsg_buffer) * SHARED_MEM_SIZE, PERMS)) == -1)
    {
        cout << "smsget - error\n";
        return -1;
    }

    int indexer_shmid;
    key_t indexer_key;

    if ((indexer_key = ftok(INDEXER_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    if ((indexer_shmid = shmget(key, sizeof(int) * 2, PERMS)) == -1)
    {
        cout << "smsget - error\n";
        return -1;
    }

    printf("Ready to Send\n");

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);

    /* Queue *q = (Queue *)tmp; */

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, std_dev);
    while (true)
    {
        double number = distribution(generator);

        struct mymsg_buffer to_push;
        to_push.price = number;
        strcpy(to_push.name, commodity);

        q->push(to_push);

        printf("Size of Queue: %lld\n", q->getSize());

        this_thread::sleep_for(chrono::milliseconds(sleep));
    }

    shmdt(q);
    return 0;
}
