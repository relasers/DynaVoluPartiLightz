#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Particle.h"
#include "GameObject.h"

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

	GLuint CreatePngTexture(char * filePath);
	GLuint CreateBmpTexture(char * filePath);
	GLuint CreateRenderTexture(std::string texture_name);
	void Test();

	void DrawPlaneMesh();
	void DrawSolidMesh();

	void SimulateParticle();
	void DrawParticle();

	void DrawTexture(GLuint textureID, GLuint x, GLuint y, GLuint width, GLuint height);

	void SetFrameBuffer();
	void ResetFrameBuffer();

	void Update();
	void CameraMove(glm::vec3 _velocity, float delta);
	Camera& GetMainCamera() { return mCamera; }

	GLuint GetTextureID(std::string _name) { return m_Texture[_name]; }
private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	GLuint CompileComputeShaders(char* filenameCS);

	void CreateGeometryDataMeshes();
	void CreateVertexBufferObjects(); 
	void CreateFrameBufferObjects();
	void CreateShaderStorageBufferObjectsForParticles();
	void CreateTextureDrawResource();
	void CreateVBOandIBOfromPlaneMesh(PlaneMesh& mesh);
	void CreateVBOandIBOofLoadedMeshes();
	void CreateSceneObjects();


	unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight);

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;

	std::unordered_map < std::string, GLuint > m_Shaders;
	std::unordered_map < std::string, GLuint > m_Texture;

	PlaneMesh mPlaneMesh;
	DirectionalLight mMainDirectionalLight;

	std::unordered_map < std::string, std::unique_ptr<Mesh> > mMeshes;
	std::unordered_map < std::string, GameObject> mGameObjects;

	std::unordered_map < std::string, GLuint > m_VBO;
	std::unordered_map < std::string, GLuint > m_IBO;

	std::unordered_map < std::string, GLuint > m_FBO;

	std::unordered_map < std::string, GLuint > m_SSBO;
	Camera mCamera;
};

