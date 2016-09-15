#ifndef MASTER_H
#define MASTER_H


#include <ctime>
#include <string>


class SummaryReport
{
	public:
		explicit SummaryReport(const char *file, const unsigned workers);
		void stop(const unsigned long cmdCount);

		void printStart() const;
		void printEnd() const;

	private:
		const std::string filename;
		const unsigned maxWorkers;
		const std::time_t startTime;
		std::time_t endTime;
		unsigned long commandsCount;
};


// ---------------------------------------------------------
class Master
{
	public:
		explicit Master(const char *fileName, const unsigned maxWorkers);

	private:
		unsigned long jobsCount;
		SummaryReport report;
};

#endif
