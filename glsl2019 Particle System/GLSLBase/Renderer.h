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

#include "CCamera.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	void KeyInputProcessing(unsigned char key);
	void MouseInput(int button, int state, int x, int y);
	void MouseMotion(int x, int y);

	GLuint CreatePngTexture(char * filePath);
	GLuint CreateBmpTexture(char * filePath);

	void CreateVertexBufferObjects();
	void CreateTexture();

	void ProxyGeometryInit(int _PointCntX, int _PointCntY);
	void QuadInit();

	// 기본 Particle System
	void CreateTextureVelocityParticle();

	void DrawVertexParticleAnim(float timeElapsed);

	// GS 사용 Particle System VBO
	void CreateParticleVertex();

	void DrawProxyGeometry();
	void DrawQuad();

	void DrawVertexTextureParticleAnim(float timeElapsed = 0.0f);

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	GLuint CompileShaders(char* filenameVS, char* filenameGS, char* filenameFS);
	unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight);

	bool m_Initialized = false;

	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	float m_Time{ 0.0f };

	// Camera
	CCamera *m_Camera = nullptr;

	// ProxyGeometry
	GLuint m_ProxyGeometryShader = 0;
	GLuint m_VBOProxyGeometry = 0;
	int m_ProxyGeometryCount = 0;

	// Quad
	GLuint m_QuadShader = 0;
	GLuint m_VBOQuad = 0;

	// SolidRect
	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	// Particle Ani
	GLuint m_VertexTextureParticleShader{ 0 };
	GLuint m_VBOVertexTextureVelocityParticle{ 0 };
	int m_vertexTextureVelocityParticleCount{ 0 };

	// Normal Particle Ani
	GLuint m_VertexParticleShader{ 0 };
	GLuint m_VBOVertexParticle{ 0 };
	int m_vertexParticleCount{ 0 };

	// Vertex Shader Texture Lookup
	GLuint m_VertexParticleVer2Shader{ 0 };
	GLuint m_TEXParticlePostion{0};
	
	// GS Particle
	GLuint m_GSParticleShader{ 0 };
	GLuint m_GSParticleVBO{ 0 };
	int m_GSParticleVBOCount{ 0 };

	// Texture
	GLuint m_TEXParticle{ 0 };
};

