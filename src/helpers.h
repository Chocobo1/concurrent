#ifndef HELPERS_H
#define HELPERS_H


#include <atomic>
#include <functional>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>


// ---------------------------------------------------------
namespace Helpers
{
	unsigned getCPUCoresLogical();
	unsigned defaultWorkersNumber();

	void runCommand(const std::string &cmd);

	std::string trimFront(const std::string &str);
	bool startswith(const std::string &parital, const std::string &full);

	bool isFence(const std::string &cmd);
	bool isComment(const std::string &cmd);
	bool isThreadLimit(const std::string &cmd);

	int parseNumber(const std::string &cmd);

	template<typename T>
	void IGNORE(const T &) {}  // for suppressing 'warning: unused parameter `x` [-Wunused-parameter]' warnings
}


// ---------------------------------------------------------
class ReadFile
{
	public:
		explicit ReadFile(const char *filename);

		std::string getLine();
		bool hasMore() const;
		unsigned long getReadCount() const;

	private:
		std::shared_ptr<std::istream> inStream;
		unsigned long readCount;
};


// ---------------------------------------------------------
#include "semaphore.h"

class JobsManager
{
	public:
		explicit JobsManager(const ssize_t maxValue);
		~JobsManager();

		template <typename F, typename... Args>
		void addJob(const F fn, const Args &... args)
		{
			// this method will BLOCK!
			sem.wait();

			const auto work = [this, fn, args...]() -> void
			{
				//fprintf(stderr, "fn: %p\n", (void *) &fn);
				//fprintf(stderr, "arg: %p\n\n", (void *) &args...);
				fn(args...);

				sem.post();
			};

			std::thread(work).detach();
		}

		ssize_t getCount();

		void setMax(const ssize_t newValue);  // NOT thread-safe!!
		ssize_t getMax();

	private:
		PseudoSemaphore sem;
};

#endif
