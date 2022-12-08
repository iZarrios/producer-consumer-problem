#define dbg(a) cout << #a << "=" << a << endl

#define PERMS 0644

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

struct my_msgbuf
{
    long mtype;
    char mtext[1024];

    my_msgbuf() : mtype(0)
    {
        memset(mtext, '\0', sizeof(mtext) + 1);
    }
};
int main(int argc, char *argv[])
{
    struct my_msgbuf buf;
    /* Msg *buf = new Msg(); */
    int msqid;
    int toend;
    key_t key;

    // get unique key
    if ((key = ftok(SHARED_MEM_NAME, 'B')) == -1)
    {
        cout << "ftok - error\n";
        return -1;
    }
    // connect to queue
    if ((msqid = msgget(key, PERMS)) == -1)
    {
        cout << "msgget - error\n";
        return -1;
    }

    /* buf->m_type = 1; // does not matter atm */
    buf.mtype = 1;

    long long count = 0;

    while (true)
    {
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1)
        {
            cout << "msgrcv" << endl;
            return -1;
        }
        printf("%lld:recvd: \"%s\"\n", count, buf.mtext);
        if (count > 10000000)
            count = 0;
        count++;
        toend = strcmp(buf.mtext, "end");
        if (toend == 0)
        {
            break;
        }
    }
    if (msgctl(msqid, IPC_RMID, nullptr) == -1)
    {
        cout << "line 77\n";
        return -1;
    }
    cout << "Done";
    return 0;
}
