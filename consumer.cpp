

#include "msg.h"
#include <bits/stdc++.h>
#include <chrono>
#include <sys/msg.h>
#include <sys/types.h>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

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
        Commidity *comm = new Commidity(s, 0, 0);
        commodities[s] = comm;
    }

    struct mymsg_buffer buf;

    int msqid;
    key_t key;

    // get unique key
    if ((key = ftok(SHARED_MEM_NAME, 'B')) == -1)
    {
        cout << "ftok - error\npossibilywrong shared memory name\n";
        return -1;
    }
    // connect to queue
    if ((msqid = msgget(key, PERMS)) == -1)
    {
        cout << "msgget - error\nCannot connect to the queue\n";
        return -1;
    }

    // struct requirement
    buf.mtype = 1;

    // Consumer Logic Starts Here

    // TODO: Error Handling
    while (true)
    {
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1)
        {
            cout << "msgrcv - error" << endl;
            return -1;
        }

        int idx;
        for (idx = 0; idx < strlen(buf.mtext); idx++)
        {
            if (buf.mtext[idx] == ':')
            {
                break;
            }
        }

        string recieved_comm_name, recieved_comm_price_str;
        double recieved_comm_price;

        for (int i = 0; i < idx; i++)
        {
            recieved_comm_name += buf.mtext[i];
        }

        // start after ':' char
        for (int i = idx + 1; i < strlen(buf.mtext); i++)
        {
            recieved_comm_price_str += buf.mtext[i];
        }

        recieved_comm_price = stod(recieved_comm_price_str);
        /* cout << recieved_comm_price; */
        Commidity *comm_recieved_to_change = commodities[recieved_comm_name];

        comm_recieved_to_change->last_price = comm_recieved_to_change->price;
        comm_recieved_to_change->last_avg_price = comm_recieved_to_change->avg_price;

        double new_avg = (commodities[recieved_comm_name]->avg_price + recieved_comm_price) / 2;
        comm_recieved_to_change->avg_price = new_avg;
        comm_recieved_to_change->price = recieved_comm_price;

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

        /* string str = "ALUMINIUM"; */
        /* string str1 = recieved_comm_name; */
        /* cout << str.length() << " " << str1.length(); */
        /* cout << str << " " << str1; */
        /* cout << endl; */
        /* cout<<commodities[recieved_comm_name]->name; */
        /* cout<<commodities["ALUMINIUM"]->name<<endl; */

        print_table(commodities, names_in_order);
    }

    // rm link to the shared memory
    if (msgctl(msqid, IPC_RMID, nullptr) == -1)
    {
        cout << "line 77\n";
        return -1;
    }

    return 0;
}
