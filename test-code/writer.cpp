#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <thread>

#define UNCHANGED 0
#define INCREASED 1
#define DECREASED -1

#include <bits/stdc++.h>
using namespace std;

class Commidity
{
  public:
    std::string name;
    double price;
    double avg_price;
    double last_price;
    double last_avg_price;
    int price_state;
    int avg_price_state;

    Commidity();
    Commidity(std::string, double, double);
};
Commidity::Commidity()
{
}
Commidity::Commidity(std::string name, double price, double avg_price)
{
    this->name = name;
    this->price = price;
    this->last_price = 0;
    this->price_state = UNCHANGED;
    this->avg_price = avg_price;
    this->last_avg_price = UNCHANGED;
}

int main()
{
    // ftok to generate unique key
    key_t key = ftok("shmfile", 65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key, sizeof(Commidity *), 0666 | IPC_CREAT);

    // shmat to attach to shared memory
    Commidity *q = (Commidity *)shmat(shmid, (void *)0, 0);

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.5, 0.25);
    while (true)
    {
        double number = distribution(generator);
        q->price = number;

        /* Commidity *to_send = new Commidity(commodity, number, 0); */
        /* cout<<q->getSize(); */
        /* q->push(to_send); */

        /* printf("%p\n", q); */
        this_thread::sleep_for(chrono::milliseconds(1000));
        /* cout << "Write Data : "; */
    }
    // detach from shared memory
    shmdt(q);

    return 0;
}
