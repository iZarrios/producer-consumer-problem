#include "msg.h"

#include <bits/stdc++.h>
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
    cout << "XDDDDDD";
    // Initial Commodities
    vector<string> names_in_order = {"GOLD",   "SILVER", "CRUDOIL", "NATURALGAS", "ALUMINIUM", "COPPER",
                                     "NICKEL", "LEAD",   "ZINC",    "MATHANOL",   "COTTON"};
    // Sort by Name
    sort(names_in_order.begin(), names_in_order.end());

    unordered_map<string, Commidity *> commodities;

    // does not have to be names in order
    for (string s : names_in_order)
    {
        Commidity *comm = new Commidity(s, 0, 0);
        commodities[s] = comm;
    }

    int shmid; // shared memory id
    key_t key; // shared memory key

    // generate unique key for the shared memeory
    if ((key = ftok(SHARED_MEM_NAME, 'B')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    // create to shared memory
    if ((shmid = shmget(key, sizeof(Queue), PERMS | IPC_CREAT)) == -1)
    {
        cout << "msget - error\n";
        return -1;
    }

    // attach to shared memory
    void *tmp = shmat(shmid, nullptr, 0);

    Queue *q = new (tmp) Queue;

    // TODO: Error Handling
    while (true)
    {
        if (q->getSize() == 0)
            continue;
        Commidity *recieved = q->pop();
        cout << recieved->name << endl;

        /* int idx; */
        /* for (idx = 0; idx < strlen(buf.mtext); idx++) */
        /* { */
        /*     if (buf.mtext[idx] == ':') */
        /*     { */
        /*         break; */
        /*     } */
        /* } */

        /* string recieved_comm_name, recieved_comm_price_str; */
        /* double recieved_comm_price; */

        /* for (int i = 0; i < idx; i++) */
        /* { */
        /*     recieved_comm_name += buf.mtext[i]; */
        /* } */

        /* // start after ':' char */
        /* for (int i = idx + 1; i < strlen(buf.mtext); i++) */
        /* { */
        /*     recieved_comm_price_str += buf.mtext[i]; */
        /* } */

        /* recieved_comm_price = stod(recieved_comm_price_str); */
        /* /1* cout << recieved_comm_price; *1/ */
        /* Commidity *comm_recieved_to_change = commodities[recieved_comm_name]; */

        /* comm_recieved_to_change->last_price = comm_recieved_to_change->price; */
        /* comm_recieved_to_change->last_avg_price = comm_recieved_to_change->avg_price; */

        /* double new_avg = (commodities[recieved_comm_name]->avg_price + recieved_comm_price) / 2; */
        /* comm_recieved_to_change->avg_price = new_avg; */
        /* comm_recieved_to_change->price = recieved_comm_price; */

        /* if (comm_recieved_to_change->last_avg_price > comm_recieved_to_change->avg_price) */
        /* { */
        /*     comm_recieved_to_change->avg_price_state = DECREASED; */
        /* } */
        /* else if (comm_recieved_to_change->last_avg_price < comm_recieved_to_change->avg_price) */
        /* { */
        /*     comm_recieved_to_change->avg_price_state = INCREASED; */
        /* } */
        /* else */
        /* { */
        /*     comm_recieved_to_change->avg_price_state = UNCHANGED; */
        /* } */

        /* if (comm_recieved_to_change->last_price > comm_recieved_to_change->price) */
        /* { */
        /*     comm_recieved_to_change->price_state = DECREASED; */
        /* } */
        /* else if (comm_recieved_to_change->last_price < comm_recieved_to_change->price) */
        /* { */
        /*     comm_recieved_to_change->price_state = INCREASED; */
        /* } */
        /* else */
        /* { */
        /*     comm_recieved_to_change->price_state = UNCHANGED; */
        /* } */

        /* print_table(commodities, names_in_order); */
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
