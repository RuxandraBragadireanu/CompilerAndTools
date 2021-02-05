#pragma once
#include <time.h>
#include <string>

enum Status {
	Normal = 0,
	Need_calibration = 1,
	Abnormal = 2,
	Power_off = 3,
	Under_repair = 4, 
	Abnormal_data = 9
};

class MeasurementInfo
{
public:
	int StationCode;
	int ItemCode;
	float MeasurementValue;
	int StatusCode;

};

