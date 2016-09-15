
#include "master.h"

#include "helpers.h"


Master::Master(const char *fileName, const unsigned maxWorkers)
	: jobsCount(0), report(fileName, maxWorkers)
{
	ReadFile reader(fileName);
	if(!reader.hasMore())
	{
		fprintf(stderr, "Open file error: %s\n", fileName);
		return;
	}

	JobsManager *jm = new JobsManager(maxWorkers);

	report.printStart();

	while(reader.hasMore())
	{
		const std::string cmd = Helpers::trimFront(reader.getLine());
		const unsigned long lineNumber = reader.getReadCount();

		if(cmd.empty())
		{
			//fprintf(stderr, "  -<L%lu> empty line\n", lineNumber);
			continue;
		}
		else if(Helpers::isFence(cmd))
		{
			// wait for previous jobs to complete, then start a new batch

			printf("  -<L%lu> fence encountered, waiting jobs...", lineNumber);
			const int limit = jm->getMax();
			delete jm;

			printf("  done, starting jobs...\n");
			jm = new JobsManager(limit);
			continue;
		}
		else if(Helpers::isThreadLimit(cmd))
		{
			const int newLimit = Helpers::parseNumber(cmd);
			if(newLimit > 0)
			{
				printf("  -<L%lu> changing concurrent to: %d\n", lineNumber, newLimit);
				jm->setMax(newLimit);
			}
			continue;
		}
		else if(Helpers::isComment(cmd))
		{
			//fprintf(stderr, "  -<L%lu> comment: %s\n", lineNumber, cmd.c_str());
			continue;
		}

		jm->addJob(Helpers::runCommand, cmd);
		printf("  -<L%lu> executing: %s\n", lineNumber, cmd.c_str());

		++jobsCount;
	}
	delete jm;

	report.stop(jobsCount);
	report.printEnd();
}


// ---------------------------------------------------------
SummaryReport::SummaryReport(const char *file, const unsigned workers)
	: filename(file != nullptr ? file : "stdin (pipe)"), maxWorkers(workers)
	, startTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
	, commandsCount(0)
{
}


void SummaryReport::stop(const unsigned long cmdCount)
{
	using clock = std::chrono::system_clock;
	endTime = clock::to_time_t(clock::now());
	commandsCount = cmdCount;
}


void SummaryReport::printStart() const
{
	printf("  Reading from: %s\n", filename.c_str());
	printf("  Detected CPU cores (logical): %u\n", Helpers::getCPUCoresLogical());
	printf("  Default concurrent jobs: %u\n", maxWorkers);
	printf("\n");
}


void SummaryReport::printEnd() const
{
	const long durTime = (long) difftime(endTime, startTime);
	int secs = durTime % 60;
	int mins = (durTime / 60) % 60;
	int hours = durTime / 3600;

	const auto toString = [](const std::time_t *time) -> std::string
	{
		char tmp[48] = {0};
		std::strftime(tmp, sizeof(tmp), "%c", std::localtime(time));
		return std::string(tmp);
	};

	printf("\n\n     ----- Summary report -----\n");
	printf("  Duration:\t\t%02d:%02d:%02d\n", hours, mins, secs);
	printf("  Start date:\t\t%s\n", toString(&startTime).c_str());
	printf("  End date:\t\t%s\n", toString(&endTime).c_str());
	printf("  Commands executed:\t%lu\n", commandsCount);
	printf("\n");
}
