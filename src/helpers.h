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
class JobsManager
{
		typedef unsigned ConCount;

	public:
		explicit JobsManager(const ConCount maxValue);
		~JobsManager();

		template <typename F, typename... Args>
		void addJob(const F fn, const Args &... args)
		{
			// this method will BLOCK!

			std::unique_lock<std::mutex> l(mtxAddJob);  // thread-safe!!
			waitUntil([this] { return concurrentCount < concurrentMax; });
			++concurrentCount;
			l.unlock();

			const auto work = [this, fn, args...]() -> void
			{
				//fprintf(stderr, "fn: %p\n", (void *) &fn);
				//fprintf(stderr, "arg: %p\n\n", (void *) &args...);
				fn(args...);
				--concurrentCount;
			};

			std::thread(work).detach();
		}

		ConCount getCount() const;

		void setMax(const ConCount newValue);  // NOT thread-safe!!
		ConCount getMax() const;

	private:
		void waitUntil(const std::function<bool()> &okPred) const;

		std::mutex mtxAddJob;
		std::atomic<ConCount> concurrentMax;
		std::atomic<ConCount> concurrentCount;
};

#endif
