#include "Semaphore.h"
namespace liuxin
{
	Semaphore::Semaphore(key_t key, int n)
	{
		CreateSem(key, n);
	}
	Semaphore::~Semaphore()
	{
		/*int sz = Semctl(m_semid, 0, GETZCNT) + Semctl(m_semid, 0, GETNCNT);
		while (sz > 0)
			sz = Semctl(m_semid, 0, GETZCNT) + Semctl(m_semid, 0, GETNCNT);
		semctl(m_semid, 0, IPC_RMID);*/
	}

	void Semaphore::acquire(int n)
	{
		Semop(m_semid, -n, SEM_UNDO);
	}
	bool Semaphore::tryAcquire(int n)
	{
		return Semop(m_semid, -n, IPC_NOWAIT);
	}
	bool Semaphore::tryAcquire(int n, int timeout)
	{
		if (timeout < 0)
			return Semop(m_semid, -n, SEM_UNDO);
		return Semtimedop(m_semid, -n, timeout);
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
			printErrorMsg("Get Semaphore fail!");
		const int MAX_TRIES = 10;
		semun arg;
		semid_ds ds;
		arg.buf = &ds;
		// 下面使用一个技巧来消除创建和初始化的竞争
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

	bool Semaphore::Semtimedop(int semid, int n, uint64_t timeout)const
	{
		sembuf sop;
		sop.sem_num = 0;
		sop.sem_op = n;
		sop.sem_flg = SEM_UNDO;
		timespec tim;
		tim.tv_sec = timeout / 1000;
		tim.tv_nsec = (timeout % 1000) * 1000000;
		if (semtimedop(semid, &sop, 1, &tim) == -1)
				return false;
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
		fprintf(stderr, "errnum: %d err_msg: %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}