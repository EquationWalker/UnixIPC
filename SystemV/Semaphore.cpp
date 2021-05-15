#include "Semaphore.h"
namespace liuxin
{
	Semaphore::Semaphore(key_t key, int n)
	{
		CreateSem(key, n);
		//printf("Construct done!\n");
	}
	Semaphore::~Semaphore()
	{
		//printf("destruct star!\n");
		int sz = Semctl(m_semid, 0, GETZCNT) + Semctl(m_semid, 0, GETNCNT);
		while (sz > 0)
			sz = Semctl(m_semid, 0, GETZCNT) + Semctl(m_semid, 0, GETNCNT);
		semctl(m_semid, 0, IPC_RMID);
		//printf("destruct done!\n");
	}

	void Semaphore::acquire(int n)
	{
		Semop(m_semid, -n, SEM_UNDO);
	}
	bool Semaphore::tryAcquire(int n)
	{
		return Semop(m_semid, -n, IPC_NOWAIT);
	}
	int Semaphore::available() const
	{
		return Semctl(m_semid, 0, GETVAL);
	}
	void Semaphore::release(int n)
	{
		Semop(m_semid, n, SEM_UNDO);
	}

	void Semaphore::CreateSem(key_t __key, int n)
	{
		m_semid = semget(__key, 1, IPC_CREAT | IPC_EXCL | 0666);
		if (m_semid != -1)
		{
			/* 事实上，Linux保证新的信号量会被初始化为0 */
			semun arg;
			arg.val = 0;
			Semctl(m_semid, 0, SETVAL, &arg);
			Semop(m_semid, n, SEM_UNDO);
			return;
		}
		m_semid = semget(__key, 1, IPC_CREAT | 0666);
		if (m_semid == -1)
			printErrorMsg("Create Semaphore fail!");
		const int MAX_TRIES = 10;
		semun arg;
		semid_ds ds;
		arg.buf = &ds;

		for (int i = 0; i < MAX_TRIES; i++)
		{
			Semctl(m_semid, 0, IPC_STAT, &arg);
			if (ds.sem_otime != 0)
				break;
			sleep(1);
		}

		return;
	}
	bool Semaphore::Semop(int semid, int n, short int flag) const
	{
		sembuf sop;
		sop.sem_num = 0;
		sop.sem_op = n;
		sop.sem_flg = flag;
		if (semop(semid, &sop, 1) == -1)
			if (flag == IPC_NOWAIT)
				return false;
			else
				printErrorMsg("Semop fail!");
		return true;
	}
	int Semaphore::Semctl(int semid, int semnum, int cmd, semun *arg) const
	{
		int res = -1;
		if (arg) res = semctl(semid, semnum, cmd, *arg);
		else res = semctl(semid, semnum, cmd);
		if (res == -1)
			printErrorMsg("Semctl fail!");
		return res;
	}
	void Semaphore::printErrorMsg(const char *msg) const
	{
		semctl(m_semid, 0, IPC_RMID);
		puts(msg);
		printf("errnum: %d err_msg: %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}