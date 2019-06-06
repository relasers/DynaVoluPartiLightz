#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"

#include <Windows.h>
#include <cstdlib>
#include <cassert>

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	mCamera.SetWindowSize(m_WindowSizeX, m_WindowSizeY);
	mCamera.SetCameraMode(CameraMatrixMode::Perspective);
	mCamera.InitializeProjMatrices();

	//Load shaders
	m_Shaders["SolidRect"] = CompileShaders("./Shaders/SolidRect.vert", "./Shaders/SolidRect.frag");
	m_Shaders["SolidMesh"] = CompileShaders("./Shaders/SolidMesh.vert", "./Shaders/SolidMesh.frag");

	m_Shaders["BasicTexture"] = CompileShaders("./Shaders/BasicTexture.vert", "./Shaders/BasicTexture.frag");
	m_Shaders["DeferedLightPass"] = CompileShaders("./Shaders/DeferedLightPass.vert", "./Shaders/DeferedLightPass.frag");

	m_Shaders["BasicParticle"] = CompileShaders("./Shaders/BasicParticle.vert", "./Shaders/BasicParticle.frag");
	m_Shaders["ParticleSimulate"] = CompileComputeShaders("./Shaders/ParticleSimulate.comp");
	m_Shaders["ParticleWorldIntensityClear"] = CompileComputeShaders("./Shaders/ParticleWorldIntensityClear.comp");
	m_Shaders["ParticleWorldIntensityUpdate"] = CompileComputeShaders("./Shaders/ParticleWorldIntensityUpdate.comp");
	//Create Geometry Data
	CreateGeometryDataMeshes();
	//Create VBOs
	CreateVertexBufferObjects();
	// Create SSBO for Particles
	CreateShaderStorageBufferObjectsForParticles();

	CreateParticleLightTextureData();
	CreateFrameBufferObjects();

	 m_Texture["OldPage"] = CreatePngTexture("./Resource/Texture/OldPage.png");
	 m_Texture["BackPage"] = CreatePngTexture("./Resource/Texture/BackPage.png");
	 m_Texture["Leather"] = CreatePngTexture("./Resource/Texture/Leather.png");

	 mMainDirectionalLight.mLightColor = glm::vec4(255.f, 244.f,214.f,255.f);
	 mMainDirectionalLight.mLightColor /= 255.f;
	 mMainDirectionalLight.mDirection = glm::normalize(glm::vec3(50,-30,0));

	 CreateSceneObjects();


}

void Renderer::CreateGeometryDataMeshes()
{
	//mPlaneMesh.BuildGeomData(50,50);
	//mMeshes["LightingCheckBoard"] = Mesh("LightingCheckBoard", std::string("./Resource/Model/LightingCheckBoard_smooth.obj"));
	mMeshes["LightingCheckBoard"] = std::make_unique<Mesh>("LightingCheckBoard", std::string("./Resource/Model/LightingCheckBoard_smooth.fbx"));
	
	//mMeshes["LightingCheckBoard"] = std::make_unique<Mesh>("LightingCheckBoard", std::string("./Resource/Model/LightingCheckBoard_smooth.fbx"));
	mMeshes["TinyDragon"] = std::make_unique<Mesh>("TinyDragon", std::string("./Resource/Model/TinyDragonOptimized.fbx"));
	mMeshes["TinyBunny"] = std::make_unique<Mesh>("TinyBunny", std::string("./Resource/Model/TinyBunny.fbx"));
	//mMeshes["TinyBuddha"] = std::make_unique<Mesh>("TinyBuddha", std::string("./Resource/Model/TinyBuddha.fbx"));




}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-0.5, -0.5, 0.f, -0.5, 0.5, 0.f, 0.5, 0.5, 0.f, //Triangle1
		-0.5, -0.5, 0.f,  0.5, 0.5, 0.f, 0.5, -0.5, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	
	//CreateVBOandIBOfromPlaneMesh(mPlaneMesh);
	CreateVBOandIBOofLoadedMeshes();
	CreateTextureDrawResource();
}

void Renderer::CreateTextureDrawResource()
{
	m_VBO["BasicTexture"] = 0;
	m_VBO["FullScreenBasicTexture"] = 0;

	// -------------------------------------------------
	// For DrawBasicTexture
	float basicTextureRect[] =
	{
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};

	glGenBuffers(1, &m_VBO["BasicTexture"]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["BasicTexture"]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(basicTextureRect), basicTextureRect, GL_STATIC_DRAW);

	// -------------------------------------------------
	// For DrawFullScreenTexture
	float fullTextureRect[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenBuffers(1, &m_VBO["FullScreenBasicTexture"]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["FullScreenBasicTexture"]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullTextureRect), fullTextureRect, GL_STATIC_DRAW);
}


void Renderer::CreateFrameBufferObjects()
{
	m_Texture["FragColor"] = 0;
	m_Texture["BrightColor"] = 0;
	m_Texture["WorldPosition"] = 0;
	m_Texture["WorldNormal"] = 0;
	m_Texture["ViewDir"] = 0;
	m_Texture["SpecData"] = 0;
	m_Texture["OutputDepth"] = 0;
	m_FBO["GBUFFER"] = 0;

	m_Texture["WorldParticleLightIntensity"] = 0;

	glGenFramebuffers(1, &m_FBO["GBUFFER"]);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO["GBUFFER"]);
	

	glGenTextures(1, &m_Texture["FragColor"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["FragColor"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture["FragColor"], 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture["FragColor"], 0);

	glGenTextures(1, &m_Texture["BrightColor"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["BrightColor"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_Texture["BrightColor"], 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_Texture["BrightColor"], 0);

	glGenTextures(1, &m_Texture["WorldPosition"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["WorldPosition"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_Texture["WorldPosition"], 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_Texture["WorldPosition"], 0);

	glGenTextures(1, &m_Texture["WorldNormal"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["WorldNormal"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_Texture["WorldNormal"], 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_Texture["WorldNormal"], 0);

	glGenTextures(1, &m_Texture["ViewDir"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["ViewDir"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_Texture["ViewDir"], 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, m_Texture["ViewDir"], 0);

	glGenTextures(1, &m_Texture["SpecData"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["SpecData"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_Texture["SpecData"], 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, m_Texture["SpecData"], 0);

	GLenum drawBuffers[6] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 ,GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(6, drawBuffers);

	glGenRenderbuffers(1, &m_Texture["OutputDepth"]);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Texture["OutputDepth"]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_WindowSizeX, m_WindowSizeY);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Texture["OutputDepth"]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::CreateShaderStorageBufferObjectsForParticles()
{
	m_SSBO["ParticlePosition"] = 0;
	m_SSBO["ParticleVelocity"] = 0;
	m_SSBO["ParticleLightColor"] = 0;
	m_SSBO["ParticleRangeAtten"] = 0;

	glGenBuffers(1,&m_SSBO["ParticlePosition"]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO["ParticlePosition"]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(ParticlePosition), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // the invalidate makes a big difference when re-writing
	ParticlePosition* points
		= (ParticlePosition*)glMapBufferRange
		(GL_SHADER_STORAGE_BUFFER, 0, 
			NUM_PARTICLES * sizeof(ParticlePosition), bufMask);
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		points[i].mPosition.x = RandomRangeFloat(0.0f, 100.0f);
		points[i].mPosition.y = RandomRangeFloat(0.0f, 100.0f);
		points[i].mPosition.z = RandomRangeFloat(0.0f, 100.0f);
		points[i].mPosition.w = 1;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &m_SSBO["ParticleVelocity"]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO["ParticleVelocity"]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(ParticleVelocity), NULL, GL_STATIC_DRAW);

	ParticleVelocity* vels
		= (ParticleVelocity*)glMapBufferRange
		(GL_SHADER_STORAGE_BUFFER, 0,
			NUM_PARTICLES * sizeof(ParticleVelocity), bufMask);
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		vels[i].mVelocity.x = RandomRangeFloat(-2.0f, 2.0f);
		vels[i].mVelocity.y = RandomRangeFloat(-2.0f, 2.0f);
		vels[i].mVelocity.z = RandomRangeFloat(-2.0f, 2.0f);
		vels[i].mVelocity.w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &m_SSBO["ParticleLightColor"]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO["ParticleLightColor"]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(ParticleLightColor), NULL, GL_STATIC_DRAW);

	ParticleLightColor* particlelightcolor
		= (ParticleLightColor*)glMapBufferRange
		(GL_SHADER_STORAGE_BUFFER, 0,
			NUM_PARTICLES * sizeof(ParticleLightColor), bufMask);
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particlelightcolor[i].mLightColor.x = RandomRangeFloat(0.5f, 1.0f);
		particlelightcolor[i].mLightColor.y = RandomRangeFloat(0.5f, 1.0f);
		particlelightcolor[i].mLightColor.z = RandomRangeFloat(0.5f, 1.0f);
		particlelightcolor[i].mLightColor.w = RandomRangeFloat(0.5,0.75);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &m_SSBO["ParticleRangeAtten"]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO["ParticleRangeAtten"]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(ParticleRangeAtten), NULL, GL_STATIC_DRAW);

	ParticleRangeAtten* particlelightatten
		= (ParticleRangeAtten*)glMapBufferRange
		(GL_SHADER_STORAGE_BUFFER, 0,
			NUM_PARTICLES * sizeof(ParticleRangeAtten), bufMask);
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particlelightatten[i].mAttenRange.x = 1.0;
		particlelightatten[i].mAttenRange.y = 0.1;
		particlelightatten[i].mAttenRange.z = 0.01;
		particlelightatten[i].mAttenRange.w = RandomRangeFloat(0.5, 2);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);



}

void Renderer::CreateVBOandIBOfromPlaneMesh(PlaneMesh& mesh)
{

	GLuint VBO;
	GLuint IBO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVerticesVector().size() * sizeof(Vertex), &mesh.GetVerticesVector()[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndicesVector().size() * sizeof(unsigned int), &mesh.GetIndicesVector()[0], GL_STATIC_DRAW);

	m_VBO["Plane"] = VBO;
	m_IBO["Plane"] = IBO;
}

void Renderer::CreateVBOandIBOofLoadedMeshes()
{
	for (auto& p : mMeshes)
	{
		GLuint VBO;
		GLuint IBO;

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, p.second->GetVerticesVector().size() * sizeof(Vertex), &p.second->GetVerticesVector().at(0), GL_STATIC_DRAW);

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, p.second->GetIndicesVector().size() * sizeof(unsigned int), &p.second->GetIndicesVector().at(0), GL_STATIC_DRAW);


		m_VBO[p.first] = VBO;
		m_IBO[p.first] = IBO;
	}
}

void Renderer::CreateSceneObjects()
{
	GameObject MainGeom;
	mGameObjects["MainGeom"] = MainGeom;
	mGameObjects["MainGeom"].SetMesh(mMeshes["LightingCheckBoard"].get());
	mGameObjects["MainGeom"].SetPosition(glm::vec3(50,10,50));
	mGameObjects["MainGeom"].SetScale(glm::vec3(1,1,1));
	mGameObjects["MainGeom"].GetMaterial().SetColor(glm::vec3(0.3, 0.3, 0.3));
	mGameObjects["MainGeom"].GetMaterial().SetSpecPower(32);
	mGameObjects["MainGeom"].GetMaterial().SetSpecFactor(1);


	for (int i = 0; i < 50; ++i)
	{
		GameObject TinyBunny;
		GameObject TinyDragon;

		std::string bunnyname = "TinyBunny" + std::to_string(i);
		std::string dragonname = "TinyDragon" + std::to_string(i);

		mGameObjects[bunnyname] = TinyBunny;
		mGameObjects[bunnyname].SetMesh(mMeshes["TinyBunny"].get());
		mGameObjects[bunnyname].SetPosition(glm::vec3(
			RandomRangeFloat(10,90), 
			10 + RandomRangeFloat(0, 80), 
			RandomRangeFloat(10, 90)));
		mGameObjects[bunnyname].SetScale(glm::vec3(1.5, 1.5, 1.5));
		mGameObjects[bunnyname].SetEulerAngle(glm::vec3(0, RandomRangeFloat(0,360),0));
		mGameObjects[bunnyname].GetMaterial().SetColor(glm::vec3(RandomRangeFloat(0, 1), RandomRangeFloat(0, 1), RandomRangeFloat(0, 1)));
		mGameObjects[bunnyname].GetMaterial().SetSpecPower(64);
		mGameObjects[bunnyname].GetMaterial().SetSpecFactor(1);

		mGameObjects[dragonname] = TinyDragon;
		mGameObjects[dragonname].SetMesh(mMeshes["TinyDragon"].get());
		mGameObjects[dragonname].SetPosition(glm::vec3(
			RandomRangeFloat(10, 90),
			10 + RandomRangeFloat(0, 80),
			RandomRangeFloat(10, 90)));
		mGameObjects[dragonname].SetScale(glm::vec3(1.2, 1.2, 1.2));
		mGameObjects[dragonname].SetEulerAngle(glm::vec3(0, RandomRangeFloat(0, 360), 0));
		mGameObjects[dragonname].GetMaterial().SetColor(glm::vec3(RandomRangeFloat(0, 1), RandomRangeFloat(0, 1), RandomRangeFloat(0, 1)));
		mGameObjects[dragonname].GetMaterial().SetSpecPower(128);
		mGameObjects[dragonname].GetMaterial().SetSpecFactor(1);
	}
	
}

void Renderer::CreateParticleLightTextureData()
{
	mWorldParticleTexture["WorldParticleLightColor"] = 0;
	mWorldParticleTexture["WorldParticleLightDirection"] = 0;

	glGenTextures(1, &mWorldParticleTexture["WorldParticleLightColor"]);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightColor"]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	// 이때는 메모리 할당만 시행한다.
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F,
		WORLD_SIZE, WORLD_SIZE, WORLD_SIZE, 0, GL_RGBA, GL_FLOAT, 0 );

	glGenTextures(1, &mWorldParticleTexture["WorldParticleLightDirection"]);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightDirection"]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	// 이때는 메모리 할당만 시행한다.
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F,
		WORLD_SIZE, WORLD_SIZE, WORLD_SIZE, 0, GL_RGBA, GL_FLOAT, 0);


}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

GLuint Renderer::CompileComputeShaders(char* filenameCS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string cs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameCS, &cs)) {
		printf("Error compiling compute shader\n");
		return -1;
	};

	// ShaderProgram 에 cs.c_str() 컴퓨트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, cs.c_str(), GL_COMPUTE_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameCS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameCS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameCS << " Shader compiling is done.\n";

	return ShaderProgram;
}
unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight)
{
	std::cout << "Loading bmp file " << imagepath << " ... " << std::endl;
	outWidth = -1;
	outHeight = -1;
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = NULL;
	fopen_s(&file, imagepath, "rb");
	if (!file)
	{
		std::cout << "Image could not be opened, " << imagepath << " is missing. " << std::endl;
		return NULL;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1E]) != 0)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1C]) != 24)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	outWidth = *(int*)&(header[0x12]);
	outHeight = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = outWidth * outHeight * 3;

	if (dataPos == 0)
		dataPos = 54;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	std::cout << imagepath << " is succesfully loaded. " << std::endl;

	return data;
}

GLuint Renderer::CreatePngTexture(char * filePath)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		lodepng_error_text(error);
		assert(error == 0);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return temp;
}

GLuint Renderer::CreateBmpTexture(char * filePath)
{
	//Load Bmp: Load file and decode image.
	unsigned int width, height;
	unsigned char * bmp
		= loadBMPRaw(filePath, width, height);

	if (bmp == NULL)
	{
		std::cout << "Error while loading bmp file : " << filePath << std::endl;
		assert(bmp != NULL);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp);

	return temp;
}

void Renderer::Test()
{
	GLuint shader = m_Shaders["SolidRect"];

	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

}

void Renderer::DrawPlaneMesh()
{
	GLuint shader = m_Shaders["PlaneMesh"];
	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mGameObjects["Plane"].GetTransform()->GetModelMatrix()[0][0]);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (GLvoid*)(sizeof(float) * 10));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLE_STRIP, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);
	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);
}

void Renderer::DrawSolidMesh(std::string _ObjectName)
{

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	GLuint shader = m_Shaders["SolidMesh"];
	glUseProgram(shader);
	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");
	GLuint cameraPos = glGetUniformLocation(shader, "u_CameraPos");
	GLuint mainDirLight_Color = glGetUniformLocation(shader, "dirLight.mLightColor");
	GLuint mainDirLight_Dir = glGetUniformLocation(shader, "dirLight.mDirection");

	GLuint materialDiffColor = glGetUniformLocation(shader, "u_material.mDiffuseColor");
	GLuint materialSpecFactor = glGetUniformLocation(shader, "u_material.mSpecularFactor");
	GLuint materialSpecPower = glGetUniformLocation(shader, "u_material.mSpecularPower");


	GLuint ActiveInterpolate = glGetUniformLocation(shader, "u_bInterPolate");
	GLuint ActiveTricubicInterpolate = glGetUniformLocation(shader, "u_bTricubicInterPolate");
	GLuint OnlyApplyLightIntensity = glGetUniformLocation(shader,"u_OnlyApplyLightIntensity");
	GLuint ApplyParticleSpecular = glGetUniformLocation(shader , "u_ApplyParticleSpecular");
	GLuint CheckLightDirection = glGetUniformLocation(shader , "u_CheckLightDirection");
	GLuint ActiveOverSampling = glGetUniformLocation(shader, "u_ActiveOverSampling");
	GLuint Tension = glGetUniformLocation(shader, "u_Tension");
	GLuint Defered = glGetUniformLocation(shader, "u_ActiveDeferredRendering");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mGameObjects[_ObjectName].GetTransform()->GetModelMatrix()[0][0]);

	glUniform3f(cameraPos, mCamera.GetCameraPos().x,
		 mCamera.GetCameraPos().y,
		 mCamera.GetCameraPos().z);
	glUniform4f(mainDirLight_Color, mMainDirectionalLight.mLightColor.x,
		mMainDirectionalLight.mLightColor.y,
		mMainDirectionalLight.mLightColor.z,
		mMainDirectionalLight.mLightColor.w);
	glUniform3f(mainDirLight_Dir, mMainDirectionalLight.mDirection.x,
		mMainDirectionalLight.mDirection.y, 
		mMainDirectionalLight.mDirection.z);

	glUniform3f(materialDiffColor, 
		mGameObjects[_ObjectName].GetMaterial().mDiffuseColor.x,
		mGameObjects[_ObjectName].GetMaterial().mDiffuseColor.y,
		mGameObjects[_ObjectName].GetMaterial().mDiffuseColor.z);

	glUniform1f(materialSpecFactor, mGameObjects[_ObjectName].GetMaterial().mSpecularFactor);
	glUniform1f(materialSpecPower, mGameObjects[_ObjectName].GetMaterial().mSpecularPower);

	glUniform1i(ActiveInterpolate, mbActiveTriInterPolate);
	glUniform1i(ActiveTricubicInterpolate, mbUsingTricubicInterpolate);
	glUniform1i(OnlyApplyLightIntensity, mbOnlyApplyLightIntensity);
	glUniform1i(ApplyParticleSpecular, mbApplyParticleSpecular);
	glUniform1i(CheckLightDirection, mbCheckLightDirection);
	glUniform1i(ActiveOverSampling, mbActiveOverSampling);
	glUniform1f(Tension, mTension);
	glUniform1i(Defered, mbActiveDeferdRendering);

	////////////////////////////////////////////////////////////////
	GLuint gTextureDirection = glGetUniformLocation(shader, "u_WorldParticleLightDirection");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightDirection"]);
	glUniform1i(gTextureDirection, 0);

	GLuint gTextureLightColor = glGetUniformLocation(shader, "u_WorldParticleLightColor");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightColor"]);
	glUniform1i(gTextureLightColor, 1);
	///////////////////////////////////////////////////////////////

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[mGameObjects[_ObjectName].GetMesh()->GetName()]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) , 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (GLvoid*)(sizeof(float) * 10));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO[mGameObjects[_ObjectName].GetMesh()->GetName()]);
	glLineWidth(2);
	
	//glPolygonMode(GL_FRONT, GL_TRIANGLES);
	glDrawElements(GL_TRIANGLES, mMeshes[mGameObjects[_ObjectName].GetMesh()->GetName()]->GetIndicesVector().size(), GL_UNSIGNED_INT, 0);

	//std::cout << "|" << m_VBO["LightingCheckBoard"] << " " << m_IBO["LightingCheckBoard"] << std::endl;
	//std::cout << mMeshes["LightingCheckBoard"].GetIndicesVector().size() << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);

	glDisable(GL_DEPTH_TEST);

	//if (mbActiveDeferdRendering)
	//{
	//	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//	if (error != GL_FRAMEBUFFER_COMPLETE)
	//	{
	//		std::cout << "bind framebuffer object error!" << std::endl;
	//	}
	//
	//	// 프레임버퍼 원상복귀
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}

}

void Renderer::SimulateParticle()
{

	if (!mbSimulatingParticle) return;
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO["ParticlePosition"]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBO["ParticleVelocity"]);
	

	GLuint shader = m_Shaders["ParticleSimulate"];

	glUseProgram(shader);
	glDispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	ClearWorldParticleTextures();
	UpdateWorldParticleTextures();

}

void Renderer::ClearWorldParticleTextures()
{
	////////////////////////////
	GLuint shader = m_Shaders["ParticleWorldIntensityClear"];

	glUseProgram(shader);

	// Activate Texture0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightDirection"]);
	glBindImageTexture(0, mWorldParticleTexture["WorldParticleLightDirection"], 0,
		GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	// 컴퓨트 셰이더에서 특정 텍스쳐에 값을 
	// 쓰기 위해서는 glBindImageTexture 를 절대 잊지 말것!
	GLuint gTextureDir = glGetUniformLocation(shader, "LightDirTex");
	glUniform1i(gTextureDir, 0);

	// Activate Texture1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightColor"]);
	glBindImageTexture(1, mWorldParticleTexture["WorldParticleLightColor"], 0,
		GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	// 컴퓨트 셰이더에서 특정 텍스쳐에 값을 
	// 쓰기 위해서는 glBindImageTexture 를 절대 잊지 말것!
	GLuint gTextureColor = glGetUniformLocation(shader, "LightColorTex");
	glUniform1i(gTextureColor, 1);


	glDispatchCompute(WORLD_SIZE / WORLD_WORK_GROUP_SIZE,
		WORLD_SIZE / WORLD_WORK_GROUP_SIZE,
		WORLD_SIZE / WORLD_WORK_GROUP_SIZE);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}



void Renderer::UpdateWorldParticleTextures()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO["ParticlePosition"]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBO["ParticleLightColor"]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SSBO["ParticleRangeAtten"]);

	GLuint shader = m_Shaders["ParticleWorldIntensityUpdate"];
	
	glUseProgram(shader);
	
	// Activate Texture0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightDirection"]);
	glBindImageTexture(0, mWorldParticleTexture["WorldParticleLightDirection"], 0,
		GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	// 컴퓨트 셰이더에서 특정 텍스쳐에 값을 
	// 쓰기 위해서는 glBindImageTexture 를 절대 잊지 말것!
	GLuint gTextureDir = glGetUniformLocation(shader, "LightDirTex");
	glUniform1i(gTextureDir, 0);


	// Activate Texture1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightColor"]);
	glBindImageTexture(1, mWorldParticleTexture["WorldParticleLightColor"], 0,
		GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
	// 컴퓨트 셰이더에서 특정 텍스쳐에 값을 
	// 쓰기 위해서는 glBindImageTexture 를 절대 잊지 말것!
	GLuint gTextureColor = glGetUniformLocation(shader, "LightColorTex");
	glUniform1i(gTextureColor, 1);

	glDispatchCompute(mNumRenderingParticle / WORK_GROUP_SIZE, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

}

void Renderer::DrawParticle()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint shader = m_Shaders["BasicParticle"];
	glUseProgram(shader);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO["ParticlePosition"]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBO["ParticleLightColor"]);

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glPointSize(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_SSBO["ParticlePosition"]);
	glVertexPointer(4, GL_FLOAT, 0, (void*)0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, mNumRenderingParticle);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_DEPTH_TEST);
}

void Renderer::LightingPass()
{
	if (!mbActiveDeferdRendering) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLuint shader{ m_Shaders["DeferedLightPass"] };

	glUseProgram(shader);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	GLuint gFragColor = glGetUniformLocation(shader, "gFragColor");
	GLuint gBrightColor = glGetUniformLocation(shader, "gBrightColor");
	GLuint gWorldPosition = glGetUniformLocation(shader, "gWorldPosition");
	GLuint gWorldNormal = glGetUniformLocation(shader, "gWorldNormal");
	GLuint gViewDir = glGetUniformLocation(shader, "gViewDir");
	GLuint gSpecData = glGetUniformLocation(shader, "gSpecData");
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture["FragColor"]);
	glUniform1i(gFragColor, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Texture["BrightColor"]);
	glUniform1i(gBrightColor, 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_Texture["WorldPosition"]);
	glUniform1i(gWorldPosition, 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_Texture["WorldNormal"]);
	glUniform1i(gWorldNormal, 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_Texture["ViewDir"]);
	glUniform1i(gViewDir, 4);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_Texture["SpecData"]);
	glUniform1i(gSpecData, 5);

	GLuint ActiveInterpolate = glGetUniformLocation(shader, "u_bInterPolate");
	GLuint ActiveTricubicInterpolate = glGetUniformLocation(shader, "u_bTricubicInterPolate");
	GLuint OnlyApplyLightIntensity = glGetUniformLocation(shader, "u_OnlyApplyLightIntensity");
	GLuint ApplyParticleSpecular = glGetUniformLocation(shader, "u_ApplyParticleSpecular");
	GLuint CheckLightDirection = glGetUniformLocation(shader, "u_CheckLightDirection");
	GLuint ActiveOverSampling = glGetUniformLocation(shader, "u_ActiveOverSampling");
	GLuint Tension = glGetUniformLocation(shader, "u_Tension");
	GLuint cameraPos = glGetUniformLocation(shader, "u_CameraPos");

	GLuint posId = glGetAttribLocation(shader, "a_Position");
	GLuint texPosId = glGetAttribLocation(shader, "a_TexPos");

	glUniform1i(ActiveInterpolate, mbActiveTriInterPolate);
	glUniform1i(ActiveTricubicInterpolate, mbUsingTricubicInterpolate);
	glUniform1i(OnlyApplyLightIntensity, mbOnlyApplyLightIntensity);
	glUniform1i(ApplyParticleSpecular, mbApplyParticleSpecular);
	glUniform1i(CheckLightDirection, mbCheckLightDirection);
	glUniform1i(ActiveOverSampling, mbActiveOverSampling);
	glUniform1f(Tension, mTension);
	glUniform3f(cameraPos, mCamera.GetCameraPos().x,
		mCamera.GetCameraPos().y,
		mCamera.GetCameraPos().z);
	GLuint gTextureDirection = glGetUniformLocation(shader, "u_WorldParticleLightDirection");
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightDirection"]);
	glUniform1i(gTextureDirection, 6);

	GLuint gTextureLightColor = glGetUniformLocation(shader, "u_WorldParticleLightColor");
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_3D, mWorldParticleTexture["WorldParticleLightColor"]);
	glUniform1i(gTextureLightColor, 7);


	GLuint mainDirLight_Color = glGetUniformLocation(shader, "dirLight.mLightColor");
	GLuint mainDirLight_Dir = glGetUniformLocation(shader, "dirLight.mDirection");

	glUniform4f(mainDirLight_Color, mMainDirectionalLight.mLightColor.x,
		mMainDirectionalLight.mLightColor.y,
		mMainDirectionalLight.mLightColor.z,
		mMainDirectionalLight.mLightColor.w);
	glUniform3f(mainDirLight_Dir, mMainDirectionalLight.mDirection.x,
		mMainDirectionalLight.mDirection.y,
		mMainDirectionalLight.mDirection.z);

	glEnableVertexAttribArray(posId);
	glEnableVertexAttribArray(texPosId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["FullScreenBasicTexture"]);

	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texPosId, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(posId);
	glDisableVertexAttribArray(texPosId);

	// Deferred Rendering 과 Forward Rendering 과 혼용하기 위함.
	// 여기서는 Deferred 렌더링 과정에서 업데이트 된 깊이 버퍼를 
	// Forward Rendering 과정에서도 사용할 수 있도록
	// 메인 프레임버퍼에 Blit 시키기 위한 과정이다.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO["GBUFFER"]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_WindowSizeX, m_WindowSizeY, 0, 0, m_WindowSizeX, m_WindowSizeY, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::DrawTexture(GLuint textureID, GLuint x, GLuint y, GLuint width, GLuint height)
{
	glViewport(x, y, width, height);

	GLuint shader{ m_Shaders["BasicTexture"] };

	glUseProgram(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLuint gTextureId = glGetUniformLocation(shader, "u_Texture");

	glUniform1i(gTextureId, 0);

	GLuint posId = glGetAttribLocation(shader, "a_Position");
	GLuint texPosId = glGetAttribLocation(shader, "a_TexPos");

	glEnableVertexAttribArray(posId);
	glEnableVertexAttribArray(texPosId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["BasicTexture"]);

	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texPosId, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(posId);
	glDisableVertexAttribArray(texPosId);


	glViewport(0, 0, m_WindowSizeX, m_WindowSizeY);
}

void Renderer::ActiveDeferedFrameBuffer()
{
	if (mbActiveDeferdRendering)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO["GBUFFER"]);
		GLenum drawBuffers[6] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 ,GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5 };
		glDrawBuffers(6, drawBuffers);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	}
}

void Renderer::BackToMainBuffer()
{
	if (mbActiveDeferdRendering)
	{
		GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (error != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "bind framebuffer object error!" << std::endl;
		}

		// 프레임버퍼 원상복귀
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::SetFrameBuffer()
{
	GLenum drawBuffers[6] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 ,GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5};
	glDrawBuffers(6, drawBuffers);
	

	//// 디버깅용
	//glClearColor(1, 1, 1, 1);
	//glClearDepth(1.f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0, 0, 0, 0);
}

void Renderer::ResetFrameBuffer()
{
	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "bind framebuffer object error!" << std::endl;
	}


	// 프레임버퍼 원상복귀
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::Update()
{


	static float deltaTime = 0;
	deltaTime += 0.016f;

	mWindowStatString.clear();

	mWindowStatString += "Num of Particle : " + std::to_string(mNumRenderingParticle) + " ";

	if (mbActiveDeferdRendering) mWindowStatString += "Defered ON | ";

	if (mbActiveTriInterPolate)
	{
		mWindowStatString += "InterPolate ON | ";
		if(mbUsingTricubicInterpolate) mWindowStatString += "Tricubic ON | ";
		if (mbOnlyApplyLightIntensity) mWindowStatString += "Disable Colored Light | ";
	}
	if (mbActiveOverSampling) mWindowStatString += "ActiveOverSampling ON | ";
	if (mbApplyParticleSpecular) mWindowStatString += "Particle Specular ON | ";
	mWindowStatString += "Tension : " + std::to_string(mTension) + " | ";

	if (!mbActiveDeferdRendering)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	for (int i = 0; i < 50; ++i)
	{
		std::string bunnyname = "TinyBunny" + std::to_string(i);
		std::string dragonname = "TinyDragon" + std::to_string(i);

		float dir = (i % 2 == 0) ? -1 : 1;

		mGameObjects[bunnyname].GetTransform()->Rotate(glm::vec3(0,1,0), i*0.0016* dir);
		mGameObjects[dragonname].GetTransform()->Rotate(glm::vec3(0, 1, 0), i*0.0016* dir);
	}
}

void Renderer::CameraMove(glm::vec3 _velocity, float delta)
{
	glm::vec3 newPosition = mCamera.GetCameraPos() + _velocity * delta;
	mCamera.SetCameraPosition(newPosition);
}

void Renderer::KeyInput(unsigned char key, int x, int y)
{
	GetMainCamera().KeyInput(key, x, y);

	switch (key) {
	case 'z' : case 'Z':
		mbActiveDeferdRendering = 1 - mbActiveDeferdRendering;
		break;
	case 'p': case 'P':
		mNumRenderingParticle = std::min(static_cast<int>(NUM_PARTICLES), mNumRenderingParticle*2);
		break;
	case 'o': case 'O':
		mNumRenderingParticle = std::max(static_cast<int>(WORK_GROUP_SIZE), static_cast<int>(mNumRenderingParticle * 0.5));

		break;

	case 'q': case 'Q':
		mbSimulatingParticle = 1 - mbSimulatingParticle;

		break;
	case 't': case 'T':
		mbActiveTriInterPolate = 1 - mbActiveTriInterPolate;

		break;
	case 'y': case 'Y':
		mbUsingTricubicInterpolate = 1 - mbUsingTricubicInterpolate;
		break;

	case 'i': case 'I':
		mbOnlyApplyLightIntensity = 1 - mbOnlyApplyLightIntensity;
		break;
	case 'u': case 'U':
		mbApplyParticleSpecular = 1 - mbApplyParticleSpecular;
		break;
	case 'k': case 'K':
		mbCheckLightDirection = 1 - mbCheckLightDirection;
		break;
	case 'm' : case 'M':
		mbActiveOverSampling = 1 - mbActiveOverSampling;
		break;
	case 'h' : case 'H':
		mTension = std::max(0.0f, mTension - 0.1f);
			break;
	case 'j': case 'J':
		mTension = std::min(mTension + 0.1f, 1.0f);
			break;
	}


}
