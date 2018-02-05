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

	void Start();
	double GetCounter();
	void Reset();
	void Show();
private:
	double	mPCFreq;
	__int64 mCounterStart;
};

#endif