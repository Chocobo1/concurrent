#include "semaphore.h"

#include <functional>


PseudoSemaphore::PseudoSemaphore(const int value)
	: totalValue(value), nowValue(value)
{
}


void PseudoSemaphore::waitExit()
{
	std::unique_lock<std::mutex> lock(valueMtx);

	cvEnd.wait(lock, [this]() -> bool
	{
		return (nowValue == totalValue);
	});  // pred must be `true` to continue
}


void PseudoSemaphore::setMaxValue(const int value)
{
	std::unique_lock<std::mutex> lock(valueMtx);

	nowValue += (value - totalValue);
	totalValue = value;
}


int PseudoSemaphore::getTotalValue()
{
	std::unique_lock<std::mutex> lock(valueMtx);

	return totalValue;
}


int PseudoSemaphore::getCount()
{
	std::unique_lock<std::mutex> lock(valueMtx);

	return (totalValue - nowValue);
}


int PseudoSemaphore::getValue()
{
	std::unique_lock<std::mutex> lock(valueMtx);

	return nowValue;  // in assembly, the return value is ready (placed in %rdi) BEFORE lock dtor is called
}


void PseudoSemaphore::wait()
{
	// lock a semaphore
	std::unique_lock<std::mutex> lock(valueMtx);

	cv.wait(lock, [this]() -> bool
	{
		return (nowValue > 0);
	});  // pred must be `true` to continue

	--nowValue;
}


void PseudoSemaphore::post()
{
	// unlock a semaphore
	std::unique_lock<std::mutex> lock(valueMtx);

	if(nowValue >= totalValue)
		return;

	++nowValue;

	if(nowValue > 0)
		cv.notify_one();

	if(nowValue == totalValue)   // less wake-ups in `waitExit()`
		cvEnd.notify_one();
}
