#include <strings.h>
#define AVG_NO 4

#include "msg.h"

#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "Queue.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

void print_table(unordered_map<string, Commidity *> commodities, vector<string> names_in_order);

int main(int argc, char *argv[])
{
    // Initial Commodities
    vector<string> names_in_order = {"GOLD",   "SILVER", "CRUDOIL", "NATURALGAS", "ALUMINIUM", "COPPER",
                                     "NICKEL", "LEAD",   "ZINC",    "MATHANOL",   "COTTON"};
    // Sort by Name
    sort(names_in_order.begin(), names_in_order.end());

    unordered_map<string, Commidity *> commodities;

    // does not have to be names in order
    for (string s : names_in_order)
    {
        Commidity *comm = new Commidity(s, 0);
        commodities[s] = comm;
    }

    union semun {
        int val;
        struct semid_ds *buf;
        ushort array[1]; // or ushort * array
    } sem_attr;

    key_t sem_key;

    int sem_mutex, sem_buffer, sem_signal;

    /* Mutual Exclusion Semaphore */

    if ((sem_key = ftok(SEM_MUTEX, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_mutex = semget(sem_key, 1, PERMS | IPC_CREAT)) == -1)
    {
        cout << "mutex_key semget- error\n";
        return -1;
    }

    sem_attr.val = 0; // locking the smeaphore for now

    if (semctl(sem_mutex, 0, SETVAL, sem_attr) == -1)
    {
        cout << "semctl mutex_sem - error\n";
        return -1;
    }

    int shmid;

    key_t key;

    // get unique key
    if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    if ((shmid = shmget(key, sizeof(struct mymsg_buffer) * SHARED_MEM_SIZE, PERMS | IPC_CREAT)) == -1)
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

    if ((indexer_shmid = shmget(indexer_key, sizeof(int) * 2, PERMS | IPC_CREAT)) == -1)
    {
        cout << "smsget - error\n";
        return -1;
    }

    int *indexer_shm;
    struct mymsg_buffer *q;

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);
    void *tmp1 = shmat(indexer_shmid, (void *)0, 0);

    char *recieved_comm_name;
    double recieved_comm_price;

    q = (struct mymsg_buffer *)tmp;

    indexer_shm = (int *)tmp1;

    *indexer_shm = 0;
    int j = 1;
    int m;

    while (true)
    {
        /* dbg(*indexer_shm); */
        if (*indexer_shm == 0)
        {
            continue;
        }

        /* cout << q[*indexer_shm].name << endl; */
        /* cout << *indexer_shm << endl; */
        *indexer_shm -= 1;

        struct mymsg_buffer recieved = q[*indexer_shm];
        /* j++; */

        recieved_comm_name = recieved.name;
        recieved_comm_price = recieved.price;

        Commidity *comm_recieved_to_change = commodities[recieved_comm_name];

        comm_recieved_to_change->last_avg_price = comm_recieved_to_change->avg_price;

        if (comm_recieved_to_change->last_prices.size() >= AVG_NO) // 4
        {
            comm_recieved_to_change->last_prices.pop();
        }
        comm_recieved_to_change->last_prices.push(recieved.price);

        comm_recieved_to_change->last_price = comm_recieved_to_change->price;

        comm_recieved_to_change->price = recieved_comm_price;

        double new_avg = 0;

        for (int i = 0; i < comm_recieved_to_change->last_prices.size(); i++)
        {
            double last_price = comm_recieved_to_change->last_prices.front();
            new_avg += last_price;
            comm_recieved_to_change->last_prices.pop();
            comm_recieved_to_change->last_prices.push(last_price);
        }
        new_avg /= AVG_NO;

        comm_recieved_to_change->avg_price = new_avg;

        if (comm_recieved_to_change->last_avg_price > comm_recieved_to_change->avg_price)
        {
            comm_recieved_to_change->avg_price_state = DECREASED;
        }
        else if (comm_recieved_to_change->last_avg_price < comm_recieved_to_change->avg_price)
        {
            comm_recieved_to_change->avg_price_state = INCREASED;
        }
        else
        {
            comm_recieved_to_change->avg_price_state = UNCHANGED;
        }

        if (comm_recieved_to_change->last_price > comm_recieved_to_change->price)
        {
            comm_recieved_to_change->price_state = DECREASED;
        }
        else if (comm_recieved_to_change->last_price < comm_recieved_to_change->price)
        {
            comm_recieved_to_change->price_state = INCREASED;
        }
        else
        {
            comm_recieved_to_change->price_state = UNCHANGED;
        }

        print_table(commodities, names_in_order);
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    shmdt(q);
    return 0;
}

void print_table(unordered_map<string, Commidity *> commodities, vector<string> names_in_order)
{
    string color_price, color_avg_price;
    string price_arrow, avg_price_arrow;
    printf("+-------------------------------------+\n");
    printf("| Currency      |  Price   | AvgPrice |\n");
    printf("+-------------------------------------+\n");
    for (string s : names_in_order)
    {
        color_price = CYAN, color_avg_price = CYAN;
        price_arrow = " ", avg_price_arrow = " ";
        Commidity *c = commodities[s];
        if (c->price_state == INCREASED)
        {
            color_price = GREEN;
            price_arrow = UP_ARROW;
        }
        if (c->price_state == DECREASED)
        {
            color_price = RED;
            price_arrow = DOWN_ARROW;
        }
        if (c->avg_price_state == INCREASED)
        {
            color_avg_price = GREEN;
            avg_price_arrow = UP_ARROW;
        }
        if (c->avg_price_state == DECREASED)
        {
            color_avg_price = RED;
            avg_price_arrow = DOWN_ARROW;
        }
        printf("|");
        printf(" %-14s", c->name.c_str());
        printf("|");
        /* c->price = 0; */
        cout << color_price;
        printf("%8.2lf%s", c->price, price_arrow.c_str());
        cout << RESET;
        /* printf("%7.2lf", c->price); */
        printf(" | ");
        cout << color_avg_price;
        printf("%8.2lf%s", c->avg_price, avg_price_arrow.c_str());
        cout << RESET;
        printf("|");

        cout << endl;
    }
    printf("+-------------------------------------+\n");
    printf("\e[1;1H\e[2J");
}
