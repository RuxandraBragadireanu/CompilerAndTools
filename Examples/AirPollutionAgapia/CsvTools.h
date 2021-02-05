#pragma once
#include <string>
#include <vector>
#include "MeasurementInfo.h"

namespace csv {
	void Read(std::string fileName, std::vector<MeasurementInfo>* measurementsCO);
}

