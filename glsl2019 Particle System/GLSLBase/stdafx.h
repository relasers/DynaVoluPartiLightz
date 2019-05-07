#pragma once

#include "targetver.h"

#include <iostream>
#include <tchar.h>

#include <chrono>

typedef std::chrono::duration<int32_t, std::ratio<1, 120>> frame;
typedef std::chrono::duration<float, std::milli> ms;
typedef std::chrono::time_point<std::chrono::steady_clock> timer;
typedef std::chrono::steady_clock STEADY_CLOCK;