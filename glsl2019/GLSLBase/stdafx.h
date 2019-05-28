#pragma once

#include "targetver.h"

// Window 헤더
#include <Windows.h>
#include <tchar.h>

// C++ 런타임 라이브러리
#include <iostream>
#include <fstream>
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

// OpenGL 라이브러리
#include "Dependencies\glew.h"
#include "Dependencies\wglew.h"

// GLMath 라이브러리
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"
#include "Dependencies\glm/gtx/quaternion.hpp"

// freeglut 라이브러리
#include "Dependencies\freeglut.h"

// assimp for import Model
#include "Dependencies\assimp\Importer.hpp"
#include "Dependencies\assimp\cimport.h"
#include "Dependencies\assimp\postprocess.h"
#include "Dependencies\assimp\scene.h"

#pragma comment(lib, "assimp.lib")


// User define
#include "Vertex.h"

extern unsigned int g_ClientWidth;
extern unsigned int g_ClientHeight;

int RAND_INT();
int RAND_INT(int min, int max);
float RAND_FLOAT();
float RAND_FLOAT(float min, float max);

constexpr ptrdiff_t LOG_SIZE = 1024;
constexpr ptrdiff_t VEC2F_SIZE = sizeof(float) * 2;
constexpr ptrdiff_t VEC3F_SIZE = sizeof(float) * 3;
constexpr ptrdiff_t VEC4F_SIZE = sizeof(float) * 4;

constexpr float FLT_PI = 3.141592f;
constexpr float DEGREE_TO_RADIAN(float degree) { return (FLT_PI * degree) / 180.0f; }
constexpr float RADIAN_TO_DEGREE(float radian) { return (180.0f * radian) / FLT_PI; }