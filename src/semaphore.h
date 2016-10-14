#ifndef SEMAPHORE_H
#define SEMAPHORE_H


#include <condition_variable>
#include <mutex>


class PseudoSemaphore
{
		// fucking stupid trivial semaphore implementation, just because some moron decides semaphore is no good for high-level use!
		// naming follows POSIX semaphore
	public:
		explicit PseudoSemaphore(const ssize_t value);

		void waitExit();  // wait until no one is locking the semaphore

		void wait();  // lock a semaphore
		void post();   // unlock a semaphore

		void setMaxValue(const ssize_t value);  // set new max value
		ssize_t getTotalValue();  // get total value

		ssize_t getCount();  // get in-use count
		ssize_t getValue();  // get value


	private:
		ssize_t totalValue;
		ssize_t nowValue;

		std::mutex valueMtx;
		std::condition_variable cv;
		std::condition_variable cvEnd;
};

#endif
