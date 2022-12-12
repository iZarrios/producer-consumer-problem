#pragma once

#define SHARED_MEM_NAME "shared_mem"
#define SHARED_MEM_SIZE 20

#define SEM_MUTEX "sem-mutex"
#define SEM_BUFFER_COUNT "sem-buffer-count"
#define SEM_SIG "sem-sig"

#define dbg(a) cout << #a << "=" << a << endl
#define UP_ARROW "↑"
#define DOWN_ARROW "↓"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"

#define PERMS 0666

#define UNCHANGED 0
#define INCREASED 1
#define DECREASED -1

#include <string.h>
#include <string>

#include <queue>

#include<time.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

void my_log_msg(char *commodity, struct tm tm_now, time_t now,std::string custom_message);

/* union my_semun { */
/*     int val; */
/*     struct semid_ds *buf; */
/*     unsigned short *array; */
/*     struct seminfo *__buf; */
/* }; */

/* union semun { */
/*     int val; */
/*     struct semid_ds *buf; */
/*     ushort array[1]; // or ushort* array */
/* } sem_attr; */

struct mymsg_buffer
{
    char name[11];
    double price;

    mymsg_buffer()
    {
        memset(name, '\0', strlen(name));
    }
};

struct shared_mem
{
    struct mymsg_buffer data[SHARED_MEM_SIZE+1];
    int buffer_index;
    int buffer_index_print;
};

class Commidity
{
  public:
    std::string name;

    double price;
    double avg_price;

    std::queue<double> last_prices;
    double last_price;
    double last_avg_price;

    int price_state;
    int avg_price_state;

    Commidity();
    Commidity(std::string, double);
};
