#include "helpers.h"


#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <iostream>
#include <regex>
#include <vector>


#ifdef _WIN32
#include <codecvt>
#include <cstdlib>
#else
#include <stdio.h>
#endif


// ---------------------------------------------------------
static const std::string parsePrefixPattern = R"(\s*(###|rem)\s*)";


// ---------------------------------------------------------
unsigned Helpers::getCPUCoresLogical()
{
	return std::thread::hardware_concurrency();
}


unsigned Helpers::defaultWorkersNumber()
{
	// leave 1 CPU core for master thread (assume master will be blocked most of the time so OS can share this CPU core)
	// workers take the rest
	// for a 4 core CPU: master & OS(1), workers(3)

	unsigned cores = getCPUCoresLogical();
	if(cores > 1)
		--cores;
	cores = std::max<unsigned>(1, cores);
	return cores;
}


void Helpers::runCommand(const std::string &cmd)
{
#if defined(_WIN32) && !defined(__MINGW32__)

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wideCmd = converter.from_bytes(cmd);
	_wsystem(wideCmd.c_str());

#else

	FILE *f = popen(cmd.c_str(), "w");
	if(f == NULL)
	{
		fprintf(stderr, "popen error: %s, command: '%s'\n", strerror(errno), cmd.c_str());
		return;
	}

	int err = pclose(f);  // wait for the command to terminate
	if(err == -1)
		fprintf(stderr, "pclose error: %s, command: '%s'\n", strerror(errno), cmd.c_str());

#endif
}


std::string Helpers::trimFront(const std::string &str)
{
	// strip front only
	std::string ret = str;

	const auto isSpace = [](const char c) -> bool
	{
		return (std::isspace(c) != 0);
	};
	const auto itEnd = std::find_if_not(ret.begin(), ret.end(), isSpace);
	ret.erase(ret.begin(), itEnd);

	return ret;
}


bool Helpers::startswith(const std::string &parital, const std::string &full)
{
	return (full.compare(0, parital.length(), parital) == 0);
}


bool Helpers::isFence(const std::string &cmd)
{
	const std::regex pattern(parsePrefixPattern + "FENCE" + parsePrefixPattern, std::regex_constants::icase);
	return std::regex_match(cmd, pattern);
}


bool Helpers::isComment(const std::string &cmd)
{
	const std::regex pattern(R"(\s*(#|rem|@).*)", std::regex_constants::icase);
	return std::regex_match(cmd, pattern);
}


bool Helpers::isThreadLimit(const std::string &cmd)
{
	const std::regex pattern(parsePrefixPattern + R"(THREADS\s*:\s*(\d)+)" + parsePrefixPattern, std::regex_constants::icase);
	return std::regex_match(cmd, pattern);
}


int Helpers::parseNumber(const std::string &cmd)
{
	// if out-of-range, return -1

	std::string str = cmd;

	const auto isDigit = [](const char c) -> bool
	{
		return (std::isdigit(c) != 0);
	};
	const auto lEnd = std::find_if(str.begin(), str.end(), isDigit);
	str.erase(str.begin(), lEnd);  // remove non-digit chars from left

	const auto rStart = std::find_if_not(str.begin(), str.end(), isDigit);
	str.erase(rStart, str.end());  // remove non-digit chars after digits

	long int val = strtol(str.c_str(), nullptr, 10);  // strtol return 0 if it fails
	if((errno == ERANGE) && ((val == LONG_MIN) || (val == LONG_MAX)))  // out of range
		return -1;

	// clamp value
	val = std::max<long int>(val, INT_MIN);
	val = std::min<long int>(val, INT_MAX);
	return val;
}


// ---------------------------------------------------------
ReadFile::ReadFile(const char *filename)
	: readCount(0)
{
	if(filename != nullptr)
		inStream.reset(new std::ifstream(filename));
	else
		inStream.reset(&std::cin, [](void *) {});
}


std::string ReadFile::getLine()
{
	std::string str;  // default empty
	std::getline(*inStream, str);

	if(inStream->good())
		++readCount;

	return str;
}


bool ReadFile::hasMore() const
{
	return inStream->good();
}


unsigned long ReadFile::getReadCount() const
{
	return readCount;
}


// ---------------------------------------------------------
JobsManager::JobsManager(const int maxValue)
	: sem(maxValue)
{
}


JobsManager::~JobsManager()
{
	// must wait until all threads exit
	sem.waitExit();
}


int JobsManager::getCount()
{
	return sem.getCount();
}


void JobsManager::setMax(const int newValue)
{
	sem.setMaxValue(newValue);
}


int JobsManager::getMax()
{
	return sem.getTotalValue();
}
