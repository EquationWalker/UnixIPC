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
    lock.acquire();
    puts("parent!");
    lock.release();
    wait(NULL);
    lock.destroy();
    return 0;
}
