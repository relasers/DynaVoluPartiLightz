#pragma once

// cmath 를 인클루드 하기 전에 선언
#define _USE_MATH_DEFINES

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <math.h>
#include <algorithm>

// 
#include "Dependencies\glew.h"
#include "Dependencies\wglew.h"
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"
#include "Dependencies\glm/gtx/quaternion.hpp"

// assimp for import Model
#include "Dependencies\assimp\Importer.hpp"
#include "Dependencies\assimp\cimport.h"
#include "Dependencies\assimp\postprocess.h"
#include "Dependencies\assimp\scene.h"

#include "Dependencies\freeglut.h"

#pragma comment(lib, "assimp.lib")

#include "MathUtility.h"

constexpr float FLT_PI = 3.141592f;
constexpr float DEGREE_TO_RADIAN(float degree) { return (FLT_PI * degree) / 180.0f; }
constexpr float RADIAN_TO_DEGREE(float radian) { return (180.0f * radian) / FLT_PI; }