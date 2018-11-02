#pragma once

#include <ctime>
#include <windows.h>
class MyClock
{
public:
	void Start()
	{
		uStartTime = clock();
	}
	void Stop()
	{
		uEndTime = clock();
	}

	void StopAndShow()
	{
		uEndTime = clock();
		std::cout<<"Time: "<<GetMiliseconds()<<std::endl;
	}

	unsigned __int64 GetMiliseconds()
	{
		return (uEndTime - uStartTime);
	}

	const char* GetTimeAsString()
	{
		static char buff[256];

		__int64 time = clock() - uStartTime;
		int miliseconds = time % 1000; time /= 1000;
		int seconds = time % 60; time /= 60;
		int minutes = time % 60; minutes /= 60;

		sprintf(buff, "%d:%d:%d", minutes, seconds, miliseconds);
		return buff;
	}

	unsigned __int64 uStartTime;
	unsigned __int64 uEndTime;
};