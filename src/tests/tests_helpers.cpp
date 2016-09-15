
#include <cstdio>
#include <climits>
#include <utility>

#include "../helpers.h"

#include "catch/single_include/catch.hpp"


TEST_CASE("Helpers::getCPUCoresLogical()", "[Helpers]")
{
	REQUIRE(Helpers::getCPUCoresLogical());
}


TEST_CASE("Helpers::defaultWorkersNumber()", "[Helpers]")
{
	const int cores = Helpers::getCPUCoresLogical();
	const int workers = Helpers::defaultWorkersNumber();

	REQUIRE(workers > 0);
	REQUIRE(workers <= cores);

	if(cores > 1)
		REQUIRE(workers == cores - 1);
}


TEST_CASE("Helpers::runCommand()", "[Helpers]")
{
	const char *fileName = "testfile.tmp";
	Helpers::runCommand(std::string("echo > ") + fileName);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	int err = remove(fileName);
	REQUIRE(err == 0);
}


TEST_CASE("Helpers::trimFront()", "[Helpers]")
{
	using PairofStrings = std::pair<const std::string, const std::string>;

	const std::vector<PairofStrings> testsTable =
	{
		// {original_string, trimmed_string}
		{"", ""},
		{" ", ""},
		{"        ", ""},

		{"a", "a"},
		{"a   ", "a   "},

		{"abc", "abc"},
		{"    abc   ", "abc   "}
	};

	for(const auto &i : testsTable)
		REQUIRE(Helpers::trimFront(i.first) == i.second);
}


TEST_CASE("Helpers::startswith()", "[Helpers]")
{
	using PairofStrings = std::pair<const std::string, const std::string>;
	using TruthEntry = std::pair<const PairofStrings, const bool>;

	const std::vector<TruthEntry> truthTable =
	{
		{{"", ""}, true},
		{{"_", ""}, false},

		{{"", "hello"}, true},
		{{"h", "hello"}, true},
		{{"he", "hello"}, true},
		{{"hello", "hello"}, true},

		{{"hello1", "hello"}, false},

		{{"_", "hello"}, false}
	};

	for(const auto &i : truthTable)
		REQUIRE(Helpers::startswith(i.first.first, i.first.second) == i.second);
}


TEST_CASE("Helpers::isFence()", "[Helpers]")
{
	using TruthEntry = std::pair<const std::string, const bool>;

	const std::vector<TruthEntry> truthTable =
	{
		{"", false},
		{" a s d f ", false},

		{"### FENCE ###", true},
		{"rem FeNCE rem", true},
		{"rem fence ###     ", true},
		{"      ###    FENCE      ###     ", true},

		{"### FENCE ### 1234567", false},
		{"1234567 ### FENCE ###", false}
	};

	for(const auto &i : truthTable)
		REQUIRE(Helpers::isFence(i.first) == i.second);
}


TEST_CASE("Helpers::isComment()", "[Helpers]")
{
	using TruthEntry = std::pair<const std::string, const bool>;

	const std::vector<TruthEntry> truthTable =
	{
		{"", false},
		{"/bin/whatever arg", false},
		{"\\bin\\whatever arg", false},

		{"rem", true},
		{"rem \\bin\\whatever arg", true},
		{" rem", true},
		{" _rem", false},
		{"_rem_abc", false},

		{"@", true},
		{"@echo off", true},

		{"#   ", true},
		{"#!/bin/sh", true},
		{"# /bin/whatever arg", true}
	};

	for(const auto &i : truthTable)
		REQUIRE(Helpers::isComment(i.first) == i.second);
}


TEST_CASE("Helpers::isThreadLimit()", "[Helpers]")
{
	using TruthEntry = std::pair<const std::string, const bool>;

	const std::vector<TruthEntry> truthTable =
	{
		{"", false},
		{"/bin/echo world", false},
		{"### THREADS: ###", false},
		{"### THREADS:", false},

		{"### THREADS: 777 ###", true},
		{"rem   THReADS:     7788   rem", true},
		{"### threads: 5566rem", true},

		{"### THREADS: 0xBEEF ###", false},
		{"### THREADS: -123 ###", false},  // only recog. positive numbers

		{"### THREADS : 123 ###", true}
	};

	for(const auto &i : truthTable)
		REQUIRE(Helpers::isThreadLimit(i.first) == i.second);
}


TEST_CASE("Helpers::parseNumber()", "[Helpers]")
{
	using TruthEntry = std::pair<const std::string, const int>;

	const std::vector<TruthEntry> truthTable =
	{
		{"", 0},
		{"/bin/echo world", 0},
		{"### THREADS: ###", 0},
		{"### THREADS: 0xBEEF ###", 0},
		{"### THREADS: E04 ###", 04},
		{"### THREADS: 23", 23},
		{"### THREADS : 23 ###", 23},
		{"0", 0},

		{"### THReADS: 1 ###", 1},
		{"    ###       THREADS:      32766       rem   ", 32766},
		{"###THREADS:3510rem", 3510},

		{"-987", 987},  // only recog. positive numbers
		{"65535", 65535},
		{"18446744073709551615", -1},
		{"9223372036854775807", -1}
	};

	for(const auto &i : truthTable)
		REQUIRE(Helpers::parseNumber(i.first) == i.second);
}


// ---------------------------------------------------------
TEST_CASE("ReadFile::ReadFile(\"file.txt\")", "[ReadFile]")
{
	ReadFile reader("sample1.txt");

	REQUIRE(reader.hasMore());

	while(reader.hasMore())
		reader.getLine();

	REQUIRE(!reader.hasMore());
}


TEST_CASE("ReadFile::ReadFile(stdin)", "[ReadFile]")
{
	ReadFile reader(nullptr);
	REQUIRE(reader.hasMore());
}


TEST_CASE("ReadFile::getReadCount()", "[ReadFile]")
{
	ReadFile reader("sample1.txt");

	for(int i = 0; i < 4; ++i)
	{
		REQUIRE(reader.getReadCount() == i);
		reader.getLine();
		REQUIRE(reader.getReadCount() == (i + 1));
	}
}


// ---------------------------------------------------------
TEST_CASE("JobsManager::JobsManager()", "[JobsManager]")
{
#ifdef _WIN32  // workaround: appveyor doesn't like big tests
	const int jobs = 100;
	const int threads = 6;
#else
	const int jobs = 1000;
	const int threads = 60;
#endif

	JobsManager *jm = new JobsManager(threads);

	std::atomic<int> fooCount(0);
	const auto foo = [&fooCount](const int x)
	{
		Helpers::IGNORE(x);
		//printf("foo(): %d\n", x);
		fooCount += 1;
	};

	std::atomic<int> barCount(0);
	const auto bar = [&barCount](const int x)
	{
		Helpers::IGNORE(x);
		barCount += 2;
	};

	for(int i = 0; i < jobs; ++i)
	{
		jm->addJob(foo, i);
		jm->addJob(bar, i);
	}

	delete jm;

	REQUIRE(fooCount == jobs);
	REQUIRE(barCount == jobs * 2);
}


TEST_CASE("JobsManager::getCount()", "[JobsManager]")
{
#ifdef _WIN32  // workaround: appveyor doesn't like big tests
	const int jobs = 100;
	const int threads = 6;
#else
	const int jobs = 1000;
	const int threads = 60;
#endif

	JobsManager *jm = new JobsManager(threads);

	bool overMax = false;
	const auto func = [&overMax, &jm]
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if(jm->getCount() > threads)
			overMax = true;
	};

	for(int i = 0; i < jobs; ++i)
		jm->addJob(func);

	delete jm;

	REQUIRE(!overMax);
}


TEST_CASE("JobsManager::setMax()", "[JobsManager]")
{
	JobsManager jm(1);

	for(int i = 0; i < 2; ++i)
	{
		const int threads = (rand() % (INT_MAX - 2)) + 1;  // avoid INT_MAX overflow and 0
		jm.setMax(threads);
		REQUIRE(jm.getMax() == threads);
	}
}


TEST_CASE("JobsManager::getMax()", "[JobsManager]")
{
	const int threads = (rand() % (INT_MAX - 2)) + 1;  // avoid INT_MAX overflow and 0
	JobsManager jm(threads);
	REQUIRE(jm.getMax() == threads);
}
