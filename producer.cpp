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
    int len;
    key_t key;

    // get unique key
    if ((key = ftok(SHARED_MEM_NAME, 'B')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    // reading shared memoruy
    if ((shmid = shmget(key, sizeof(Queue), PERMS)) == -1)
    {
        cout << "msget - error\n";
        return -1;
    }

    printf("Ready to Send\n");

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);
    /* auto tmp = nullptr; */
    if (tmp == nullptr)
    {
        printf("shmat -error");
        return -1;
    }

    Queue *q = new (tmp) Queue;

    Commidity *tmp_comm = new Commidity("hi", 0, 0);
    Commidity *tmp_comm1 = new Commidity("hi111", 0, 0);

    q->push(tmp_comm);
    q->push(tmp_comm1);
    q->push(tmp_comm);
    q->push(tmp_comm);
    q->push(tmp_comm);
    cout << q->getSize() << endl;
    cout << q->front()->name << endl;
    q->pop();
    cout << q->front()->name << endl;

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, std_dev);
    while (true)
    {
        double number = distribution(generator);

        cout << "no seg fault here prob";
        Commidity *to_send = new Commidity(commodity, number, 0);
        q->push(to_send);

        printf("Sent\n");

        this_thread::sleep_for(chrono::milliseconds(sleep));
    }

    shmdt(q);

    return 0;
}
