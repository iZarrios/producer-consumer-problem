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

    int shmid; // shared memory id
    key_t key; // shared memory key

    // generate unique key for the shared memeory
    if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    // create to shared memory
    /* if ((shmid = shmget(key, sizeof(Queue), PERMS | IPC_CREAT)) == -1) */
    if ((shmid = shmget(key, sizeof(Queue *) * 1024 + 20, PERMS | IPC_CREAT)) == -1)
    {
        cout << "shmget - error\n";
        return -1;
    }

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);

    Queue *q = (Queue *)tmp;

    q->clear();

    char *recieved_comm_name;
    double recieved_comm_price;
    long long count = 0;

    while (true)
    {
        if (q->getSize() == 0)
        {
            continue;
        }

        struct mymsg_buffer recieved = q->pop();

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
