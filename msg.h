#pragma once
#define SHARED_MEM_NAME "shared_mem"
#define SHARED_MEM_SIZE 1024

#define INDEXER_NAME "indexer"

#define SEM_MUTEX "sem-mutex"
#define SEM_BUFFER_COUNT "sem-buffer-count"
#define SEM_SIG "sem-sig"

#define dbg(a) cout << #a << "=" << a << endl
#define UP_ARROW "↑"
#define DOWN_ARROW "↓"

// the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#define PERMS 0644

#define UNCHANGED 0
#define INCREASED 1
#define DECREASED -1

#include <string.h>
#include <string>

#include <queue>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

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

/* struct shared_mem { */
/*     mymsg_buffer */

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
