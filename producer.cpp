#include <string>
#define dbg(a) cout << #a << "=" << a << endl
#define PERMS 0644

#include "msg.h"
#include <bits/stdc++.h>
#include <chrono>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
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

    /* Msg *buf = new Msg(commodity, -1); */
    struct mymsg_buffer buf;
    int msqid;
    int len;
    key_t key;

    // get unique key

    if ((key = ftok(SHARED_MEM_NAME, 'B')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }

    // connect to queue
    // shmat (with size)
    if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1)
    {
        cout << "msget - error\n";
        return -1;
    }
    printf("Ready to Send\n");
    /* buf->m_type = 1; // does not matter atm */
    buf.mtype = 1;

    /* dbg(commodity); */
    /* dbg(mean); */
    /* dbg(std_dev); */
    /* dbg(sleep); */

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, std_dev);
    while (true)
    {
        double number = distribution(generator);
        sprintf(buf.mtext, "%s:%2lf", commodity, number);

        len = strlen(buf.mtext);
        if (buf.mtext[len - 1] == '\n')
        {
            buf.mtext[len - 1] = '\0';
        }
        // have pointer send to it
        // 
        if (msgsnd(msqid, &buf, len + 1, 0) == -1)
        {
            cout << "msgnd - error\n";
            return -1;
        }
        this_thread::sleep_for(chrono::milliseconds(sleep));
    }

    if (msgctl(msqid, IPC_RMID, nullptr) == -1)
    {
        cout << "line 77\n";
        return -1;
    }
    return 0;
}
