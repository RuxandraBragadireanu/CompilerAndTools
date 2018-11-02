#pragma once

#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H

#include <stdio.h>

// Comment this to disable scheduler logs - ON DISTRIBUTEDDEBUG is enabled by default
//#define USE_SCHEDULER_LOGS


#ifdef USE_SCHEDULER_LOGS
#define LOG(params)	printf params
#else
#define LOG(params) {}
#endif


#endif SCHEDULER_UTILS_H