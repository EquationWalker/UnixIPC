#include "Semaphore.h"
using namespace std;
using namespace liuxin;
using namespace std;
void aa();

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
union semun
{
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
#if defined(__linux__)
    struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
#endif
};

void aa()
{

    int id = semget(12234, 1, IPC_CREAT | 0666);
    semun a;
    a.val = 1;
    if (-1 == semctl(id, 0, SETVAL, a))
        puts("sasa");
    printf("id :%d\n", id);

    sembuf arg;
    arg.sem_flg = SEM_UNDO;
    arg.sem_num = 0;
    arg.sem_op = 2;

    if (-1 == semop(id, &arg, 1))
        puts(strerror(errno));
}
