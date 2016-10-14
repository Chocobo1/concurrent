
#include <cmath>
#include <thread>

#include "../semaphore.h"

#include "catch/single_include/catch.hpp"


TEST_CASE("PseudoSemaphore::waitExit()", "[PseudoSemaphore]")
{
	const auto f = [](const int RESOURCES, const int CONSUMERS)
	{
		PseudoSemaphore s(RESOURCES);

		const auto f = [&s]()
		{
			s.wait();
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			s.post();
		};

		REQUIRE(s.getValue() == RESOURCES);

		for(int i = 0; i < CONSUMERS; ++i)
			std::thread(f).detach();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		REQUIRE(s.getValue() == std::max(RESOURCES - CONSUMERS, 0));

		s.waitExit();
		REQUIRE(s.getValue() == RESOURCES);
	};

	f(10, 5);
	f(5, 10);
}


TEST_CASE("PseudoSemaphore::wait()", "[PseudoSemaphore]")
{
	PseudoSemaphore s(10);
	s.wait();
	REQUIRE(s.getValue() == 9);
	s.wait();
	REQUIRE(s.getValue() == 8);
	s.wait();
	REQUIRE(s.getValue() == 7);
}


TEST_CASE("PseudoSemaphore::post()", "[PseudoSemaphore]")
{
	const auto positive = []()
	{
		PseudoSemaphore s(10);
		s.wait();
		s.wait();
		s.wait();
		REQUIRE(s.getValue() == 7);
		s.post();
		s.post();
		REQUIRE(s.getValue() == 9);
		s.post();
		REQUIRE(s.getValue() == 10);
		s.post();
		REQUIRE(s.getValue() == 10);
	};
	positive();

	const auto negative = []()
	{
		PseudoSemaphore s(-3);
		s.post();
		REQUIRE(s.getValue() == -3);
	};
	negative();
}


TEST_CASE("PseudoSemaphore::setMaxValue()", "[PseudoSemaphore]")
{
	const auto f = [](const int VALUE, const int NEWVALUE)
	{
		PseudoSemaphore s(VALUE);
		REQUIRE(s.getValue() == VALUE);
		s.setMaxValue(NEWVALUE);
		REQUIRE(s.getValue() == NEWVALUE);
	};

	f(10, 20);
	f(10, -3);
	f(-4, 7);

	const auto f2 = []()
	{
		const int RESOURCES = 20;
		const int CONSUMERS = 15;

		PseudoSemaphore s(RESOURCES);
		REQUIRE(s.getValue() == RESOURCES);

		for(int i = 0; i < CONSUMERS; ++i)
			s.wait();

		REQUIRE(s.getValue() == 5);

		s.setMaxValue(10);
		REQUIRE(s.getValue() == -5);
		s.post();
		REQUIRE(s.getValue() == -4);

		s.setMaxValue(30);
		REQUIRE(s.getValue() == 16);
		s.post();
		REQUIRE(s.getValue() == 17);
	};

	f2();
}


TEST_CASE("PseudoSemaphore::getTotalValue()", "[PseudoSemaphore]")
{
	PseudoSemaphore s(5);
	REQUIRE(s.getTotalValue() == 5);

	s.setMaxValue(10);
	REQUIRE(s.getTotalValue() == 10);

	s.setMaxValue(-1);
	REQUIRE(s.getTotalValue() == -1);
}


TEST_CASE("PseudoSemaphore::getCount()", "[PseudoSemaphore]")
{
	PseudoSemaphore s(10);
	REQUIRE(s.getCount() == 0);

	s.wait();
	REQUIRE(s.getCount() == 1);

	s.wait();
	s.wait();
	s.wait();
	s.post();
	REQUIRE(s.getCount() == 3);
}


TEST_CASE("PseudoSemaphore::getValue()", "[PseudoSemaphore]")
{
	const auto f = [](const int v)
	{
		PseudoSemaphore s(v);
		REQUIRE(s.getValue() == v);
	};

	f(-15);
	f(0);
	f(100);
}
