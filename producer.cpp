#include <string>
#define dbg(a) cout << #a << "=" << a << endl
#define PERMS 0644

#include "msg.h"
#include <bits/stdc++.h>
#include <chrono>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

struct my_msgbuf
{
    long mtype;
    char mtext[1024];

    my_msgbuf() : mtype(0)
    {
        memset(mtext, '\0', sizeof(mtext) + 1);
    }
};

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
    struct my_msgbuf buf;
    int msqid;
    int len;
    key_t key;

    // get unique key

    if ((key = ftok(SHARED_MEM_NAME, 'B')) == -1)
    {
        cout << "frtok - error\n";
        return -1;
    }
    // shmget,shmat
    //
    // connect to queue
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
        sprintf(buf.mtext, "%s:%.3f", commodity, number);
        /* cin.getline(buf.mtext, sizeof(buf.mtext)); */
        len = strlen(buf.mtext);
        if (buf.mtext[len - 1] == '\n')
        {
            buf.mtext[len - 1] = '\0';
        }
        if (msgsnd(msqid, &buf, len + 1, 0) == -1)
        {
            cout << "msgnd - error\n";
            return -1;
        }
        this_thread::sleep_for(chrono::milliseconds(sleep));
    }
    /* while (true) */
    /* { */
    /*     double number = distribution(generator); */
    /*     buf->price = number; */
    /*     string s = buf->get_msg(); */
    /*     /1* dbg(number); *1/ */
    /*     this_thread::sleep_for(chrono::milliseconds(sleep)); */
    /*     if (msgsnd(msqid, &buf, sizeof(s), 0) == -1) */
    /*     { */
    /*         cout << "msgnd - error\n"; */
    /*         return -1; */
    /*     } */
    /* } */
    strcpy(buf.mtext, "end");
    len = strlen(buf.mtext);
    if (msgsnd(msqid, &buf, len + 1, 0) == -1)
    {
        cout << "msgnd - error\n";
        return -1;
    }

    if (msgctl(msqid, IPC_RMID, nullptr) == -1)
    {
        cout << "line 77\n";
        return -1;
    }
    cout << "Done";
    return 0;
}
