#pragma once
#include "Light.h"

class Camera;
class Mesh;
class GameObject;

class Renderer {
public:
	enum class OBJECT_TYPE{ CUBE };
	struct VBO {
		GLuint buf;
		GLuint count;
		VBO() : buf(0), count(0) {}
	};
	struct TFO {
		GLuint tf;
		GLuint buf;
		GLuint count;
		TFO() : tf(0), buf(0), count(0) {}
	};

public:
	Renderer(unsigned int client_width, unsigned int client_height);
	~Renderer();

	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

	GLuint CreatePngTexture(const char* filePath);
	GLuint CreateBmpTexture(const char* filePath);
	   
	void DrawRect(float size);
	void DrawPageTurning(const glm::vec3& axis, float radius, float run_rate);
	void DrawParticleCDTexture();
	void DrawParticles();
	void DrawToFrameBufferTest(GLuint rander_target_id);

	void DumpTexture(GLuint tex_id, GLuint x, GLuint y, GLuint w, GLuint h);

	void DrawCubeMesh();
	void DrawCubeMeshCDTexture();

private:
	void Init(unsigned int client_width, unsigned int client_height);

	bool ReadFile(std::string& out_data, const char* filename);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(const char* filenameVS, const char* filenameFS);
	GLuint CompileShaders(const char* filenameVS, const char* filenameGS, const char* filenameFS);

	unsigned char* loadBMPRaw(const char* imagepath, unsigned int& outWidth, unsigned int& outHeight);
	
	void CreateGeometryMeshDatas();

	void CreateRectVBO(); 
	void CreateProxyGeometryVBO();
	void CreateParticleTFO();

	void CreateVBOandIBObyLoadMeshs();
	void CreateSceneObjects();

	void CreateFrameResources();

private:
	bool						m_bInit;
	
	unsigned int				m_ClientWidth;
	unsigned int				m_ClientHeight;

	Camera*						m_pCamera;

	VBO							m_vboRect;
	VBO							m_vboProxyGeo;

	GLuint						m_SolidRectShader;
	GLuint						m_PageTurningShader;
	GLuint						m_TextureDumpShader;
	GLuint						m_ParticleShader;
	GLuint						m_ParticleCDShader;

	GLuint						m_nTextures;
	GLuint*						m_pTextures;

	GLuint						m_nTFOs;
	TFO*						m_pTFOs;
	int							m_CurrTFO;

	GLuint						m_nFBOs;
	GLuint*						m_pFBOs;

	GLuint						m_nRenderTargets;
	GLuint*						m_pRenderTargets;

	GLuint						m_nDepthBuffers;
	GLuint*						m_pDepthBuffers;

	//Mesh Object
	GLuint						m_MeshShader;
	GLuint						m_MeshCDShader;

	std::unordered_map<OBJECT_TYPE, std::unique_ptr<Mesh>>	m_Meshes;
	std::unordered_map<OBJECT_TYPE, GameObject>				m_GameObjects;
	std::unordered_map<OBJECT_TYPE, GLuint>					m_VBO;
	std::unordered_map<OBJECT_TYPE, GLuint>					m_IBO;

	// Light
	DirectionalLight m_MainDirectionalLight;

};

