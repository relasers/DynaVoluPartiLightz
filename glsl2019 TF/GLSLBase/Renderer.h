#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"
#include "Dependencies\wglew.h"
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	void CreateTFVBO();
	void CreateTFwithGSVBO();

	GLuint CreatePngTexture(char * filePath);
	GLuint CreateBmpTexture(char * filePath);
	   
	void Test();

	void TransformFeedbackFunc();
	void TransformFeedbackWithGSFunc();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	GLuint CompileShadersNoLink(char* filenameVS, char* filenameFS);
	GLuint CompileShadersNoLink(char* filenameVS, char* filenameGS, char* filenameFS);
	void CreateVertexBufferObjects();
	unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight);

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;
	
	GLuint m_TFShader{ 0 };
	GLuint m_TFVBO{ 0 };
	GLuint m_TFTBO{ 0 };

	GLuint m_TFwithGSShader;
	GLuint m_TFwithGSVBO{ 0 };
	GLuint m_TFwithGSTBO{ 0 };
};

