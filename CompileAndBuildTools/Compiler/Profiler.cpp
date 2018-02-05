#include "Profiler.h"
#include <windows.h>
#include <iostream>

using namespace std;

void Timer::Start()
{
	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	mPCFreq = double(li.QuadPart)/1000.0;

	QueryPerformanceCounter(&li);
	mCounterStart = li.QuadPart;
}

double Timer::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart-mCounterStart)/mPCFreq;
}

void Timer::Reset()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	mCounterStart = li.QuadPart;
}

void Timer::Show()
{
	cout << GetCounter() << endl;
}

/*
int main()
{
	Timer timer;
	timer.StartCounter();
	Sleep(1000);
	cout << timer.GetCounter() <<"\n";
	return 0;
}
*/