#include "Semaphore.h"
using namespace std;
using namespace liuxin;

int main()
{
     pid_t pid = fork();
    if (!pid)
    {
        Semaphore lock(ftok(".", 0));
        puts("child start P");
        lock.acquire();
        printf("child\n");
        lock.release();
        return 0;
    }
    Semaphore lock(ftok(".", 0));
    //printf("1 av :%d\n", lock.available());
    
    lock.acquire();
    puts("parent!");
   // printf("2 av :%d\n", lock.available());
    lock.release();
    //printf("3 av :%d\n", lock.available());

    //aa();

    return 0;
}
