#pragma once
#include "common.h"

namespace liuxin
{
	class Semaphore final
	{
	public:
		explicit Semaphore(key_t key, int n = 1);
		~Semaphore();
		void acquire(int n = 1);
		bool tryAcquire(int n = 1);
		// Note: This function is equivalent to call acquire() if timeout is negative.
		bool tryAcquire(int n, int timeout);
		int available() const;
		void release(int n = 1);

		Semaphore(const Semaphore &) = delete;
		Semaphore operator=(const Semaphore &) = delete;
		// Semaphore(const Semaphore &&) = delete;
		// Semaphore operator=(const Semaphore &&) = delete;

	private:
		union semun
		{
			int val;			   /* Value for SETVAL */
			struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
			unsigned short *array; /* Array for GETALL, SETALL */
#if defined(__linux__)
			struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
#endif
		};
		int m_semid;

		void CreateSem(key_t __key, int n);
		bool Semop(int semid, int n, short int flag) const;
		bool Semtimedop(int semid, int n, uint64_t timeout)const;
		int Semctl(int semid, int semnum, int cmd, semun* arg = NULL) const;
		void printErrorMsg(const char *msg)const;
	};
}
