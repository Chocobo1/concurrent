#ifndef SEMAPHORE_H
#define SEMAPHORE_H


#include <condition_variable>
#include <mutex>


class PseudoSemaphore
{
		// fucking stupid trivial semaphore implementation, just because some moron decides semaphore is no good for high-level use!
		// naming follows POSIX semaphore
	public:
		explicit PseudoSemaphore(const int value);

		void waitExit();  // wait until no one is locking the semaphore

		void wait();  // lock a semaphore
		void post();   // unlock a semaphore

		void setMaxValue(const int value);  // set new max value
		int getTotalValue();  // get total value

		int getCount();  // get in-use count
		int getValue();  // get value


	private:
		int totalValue;
		int nowValue;

		std::mutex valueMtx;
		std::condition_variable cv;
		std::condition_variable cvEnd;
};

#endif
