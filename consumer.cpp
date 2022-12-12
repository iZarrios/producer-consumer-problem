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
void signalHandler(int sig_num);
key_t sem_key1;
key_t sem_key2;
key_t sem_key3;
int shmid;
struct shared_mem *q;

void print_table(unordered_map<string, Commidity *> commodities, vector<string> names_in_order);

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);
    /* while (true); */
    // Initial Commodities
    vector<string> names_in_order = {"GOLD",   "SILVER", "CRUDOIL", "NATURALGAS", "ALUMINIUM", "COPPER",
                                     "NICKEL", "LEAD",   "ZINC",    "METHANOL",   "COTTON"};
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

    key_t sem_key1;
    key_t sem_key2;
    key_t sem_key3;

    int sem_mutex, sem_buffer, sem_signal;

    /* Mutual Exclusion Semaphore */

    if ((sem_key1 = ftok(SEM_MUTEX, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_mutex = semget(sem_key1, 1, PERMS | IPC_CREAT)) == -1)
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

    key_t key;

    // get unique key
    if ((key = ftok(SHARED_MEM_NAME, 'A')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }
    else
    {
        cout << "Got key shared memory\n";
    }

    if ((shmid = shmget(key, sizeof(struct shared_mem), PERMS | IPC_CREAT)) == -1)
    {
        cout << "line 84:shmget - error\n";
        cout << "Doom\n";
        return -1;
    }
    else
    {
        cout << "Got id shared memory\n";
    }

    // attach to shared memory
    void *tmp = shmat(shmid, (void *)0, 0);

    char *recieved_comm_name;
    double recieved_comm_price;

    q = (struct shared_mem *)tmp;

    if (q == (struct shared_mem *)-1)
    {
        printf("Q is -1\n");
        return -1;
    }
    else
    {
        printf("Q is not null\n");
    }

    q->buffer_index = 0;
    q->buffer_index_print = 0;

    /* Semaphore Buffer Count */
    if ((sem_key2 = ftok(SEM_BUFFER_COUNT, 'A')) == -1)
    {
        cout << "mutex_key frtok - error\n";
        return -1;
    }
    if ((sem_buffer = semget(sem_key2, 1, PERMS | IPC_CREAT)) == -1)
    {
        cout << "sem_buffer semget- error\n";
        return -1;
    }
    sem_attr.val = SHARED_MEM_SIZE; // intially, all spaces are available (1024 for now)

    if (semctl(sem_buffer, 0, SETVAL, sem_attr) == -1)
    {
        cout << "semctl buffer_sem - error\n";
        return -1;
    }

    /* Signal Semaphore */

    if ((sem_key3 = ftok(SEM_SIG, 'A')) == -1)
    {
        cout << "sem_sig frtok - error\n";
        return -1;
    }
    if ((sem_signal = semget(sem_key3, 1, PERMS | IPC_CREAT)) == -1)
    {
        cout << "sem-sig semget- error\n";
        return -1;
    }

    sem_attr.val = 0; // intially, all spaces are empty to produce in(1024 for now)

    if (semctl(sem_signal, 0, SETVAL, sem_attr) == -1)
    {
        cout << "semctl sem_signal - error\n";
        return -1;
    }

    sem_attr.val = 1;
    if (semctl(sem_mutex, 0, SETVAL, sem_attr) == -1)
    {
        cout << "semctl unlocking - error\n";
        return -1;
    }

    struct sembuf sem_buf[1];

    sem_buf[0].sem_num = 0;
    sem_buf[0].sem_op = 0;
    sem_buf[0].sem_flg = 0;

    while (true)
    {
        sem_buf[0].sem_op = -1;

        if (semop(sem_signal, sem_buf, 1) == -1)
        {
            printf("Semop sem_signal in while\n");
            return -1;
        }
        struct mymsg_buffer recieved = q->data[q->buffer_index];

        if (strlen(recieved.name) == 0)
        {
            continue;
        }

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

        q->buffer_index_print++;
        if (q->buffer_index_print == SHARED_MEM_SIZE)
        {
            q->buffer_index_print = 0;
        }

        sem_buf[0].sem_op = 1;

        if (semop(sem_buffer, sem_buf, 1) == -1)
        {
            printf("unlocking sem buffer cnsumer line 240");
            return -1;
        }
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
    /* printf("Done Cleaning\n"); */
    cout << endl;
    exit(0);
    signal(SIGINT, signalHandler);
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
