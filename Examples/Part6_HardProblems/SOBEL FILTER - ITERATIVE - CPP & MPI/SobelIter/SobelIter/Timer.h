#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	Timer()
		: mPCFreq(0)
		, mCounterStart(0)
	{
	}

	void StartCounter();
	double GetCounter();
	void Reset();
private:
	double	mPCFreq;
	__int64 mCounterStart;
};

#endif