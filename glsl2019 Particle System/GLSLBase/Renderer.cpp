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

void Renderer::KeyInputProcessing(unsigned char key)
{
	m_Camera->KeyInputProcessing(key);
}

void Renderer::MouseInput(int button, int state, int x, int y)
{
	m_Camera->MouseInput(button, state, x, y);
}

void Renderer::MouseMotion(int x, int y)
{
	m_Camera->MouseMotion(x, y);
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_ProxyGeometryShader = CompileShaders("./Shaders/ProxyGeometry.vs", "./Shaders/ProxyGeometry.fs");
	m_QuadShader = CompileShaders("./Shaders/Quad.vs", "./Shaders/Quad.fs");
	m_VertexTextureParticleShader = CompileShaders("./Shaders/VertexTextureParticleWithVelocity.glvs", "./Shaders/VertexTextureParticleWithVelocity.glfs");
	m_VertexParticleShader = CompileShaders("./Shaders/VertexParticle.glvs", "./Shaders/VertexParticle.glgs", "./Shaders/VertexParticle.glfs");
	m_VertexParticleShader = CompileShaders("./Shaders/VertexParticle.glvs", "./Shaders/VertexParticle.glfs");
	m_VertexParticleVer2Shader = CompileShaders("./Shaders/VertexParticle_ver2.glvs", "./Shaders/VertexParticle_ver2.glgs", "./Shaders/VertexParticle_ver2.glfs");

	// Camera
	m_Camera = new CCamera();
	m_Camera->SetCameraType(CameraType::Perpective);
	m_Camera->SetCameraPos(glm::vec3(0, 0, 1));
	m_Camera->SetCameraLookatPos(glm::vec3(0, 0, -1));
	m_Camera->SetCameraUp(glm::vec3(0.0f, 1.0f, 0.0f));
	m_Camera->Initalize(windowSizeX, windowSizeY);
	
	//Create VBOs
	CreateVertexBufferObjects();

	//Create Textures
	CreateTexture();
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

	//Proxy
	ProxyGeometryInit(8, 8);

	//Quad
	QuadInit();

	//
	CreateTextureVelocityParticle();

	// 
	CreateParticleVertex();
}

void Renderer::CreateTexture()
{
	m_TEXParticle = CreatePngTexture("./Resource/Texture/dirtyFlash.png");
	m_TEXParticlePostion = CreatePngTexture("./Resource/Texture/Test.png");
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

GLuint Renderer::CompileShaders(char * filenameVS, char * filenameGS, char * filenameFS)
{

	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, gs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	if (!ReadFile(filenameGS, &gs)) {
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

	AddShader(ShaderProgram, gs.c_str(), GL_GEOMETRY_SHADER);

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

void Renderer::ProxyGeometryInit(int _PointCntX, int _PointCntY)
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	int pointCountX = _PointCntX;
	int pointCountY = _PointCntY;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	m_ProxyGeometryCount = (pointCountX - 1)*(pointCountY - 1) * 2 * 3;
	float* point = new float[pointCountX*pointCountY * 2];
	float* vertices = new float[m_ProxyGeometryCount * 3];

	//Prepare points
	for (int x = 0; x < pointCountX; x++)
	{
		for (int y = 0; y < pointCountY; y++)
		{
			point[(y*pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y*pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex] = point[(y*pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y*pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1)*pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1)*pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1)*pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1)*pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;

			//Triangle part 2
			vertices[vertIndex] = point[(y*pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y*pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[(y*pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y*pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1)*pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1)*pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}

	glGenBuffers(1, &m_VBOProxyGeometry);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOProxyGeometry);
	glBufferData(GL_ARRAY_BUFFER, m_ProxyGeometryCount * sizeof(float) * 3, vertices, GL_STATIC_DRAW);
}

void Renderer::QuadInit()
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	float* point = new float[8];
	float* vertices = new float[18];

	//Prepare points
	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			point[(y * 2 + x) * 2] = basePosX + width * x;
			point[(y * 2 + x) * 2 + 1] = basePosY + height * y;
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < 2 - 1; x++)
	{
		for (int y = 0; y < 2 - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex++] = point[(y*2 + x) * 2];
			vertices[vertIndex++] = point[(y*2 + x) * 2 + 1];
			vertices[vertIndex++] = 0.f;
			vertices[vertIndex++] = point[((y + 1)*2 + (x + 1)) * 2];
			vertices[vertIndex++] = point[((y + 1)*2 + (x + 1)) * 2 + 1];
			vertices[vertIndex++] = 0.f;
			vertices[vertIndex++] = point[((y + 1)*2 + x) * 2];
			vertices[vertIndex++] = point[((y + 1)*2 + x) * 2 + 1];
			vertices[vertIndex++] = 0.f;

			//Triangle part 2
			vertices[vertIndex++] = point[(y * 2 + x) * 2];
			vertices[vertIndex++] = point[(y * 2 + x) * 2 + 1];
			vertices[vertIndex++] = 0.f;
			vertices[vertIndex++] = point[(y * 2 + (x + 1)) * 2];
			vertices[vertIndex++] = point[(y * 2 + (x + 1)) * 2 + 1];
			vertices[vertIndex++] = 0.f;
			vertices[vertIndex++] = point[((y + 1)* 2 + (x + 1)) * 2];
			vertices[vertIndex++] = point[((y + 1)* 2 + (x + 1)) * 2 + 1];
			vertices[vertIndex++] = 0.f;
		}
	}

	glGenBuffers(1, &m_VBOQuad);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOQuad);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float) * 3, vertices, GL_STATIC_DRAW);
}

void Renderer::CreateTextureVelocityParticle()
{
	int particleCount = 500;
	float particleSize = 0.05f;
	float* particleVertices = new float[particleCount * 2 * 3 * (3 + 2 + 4)];
	int particleFloatCount = particleCount * 2 * 3 * (3 + 2 + 4);
	m_vertexTextureVelocityParticleCount = particleCount * 2 * 3;

	int particleVertIndex = 0;

	for (int i = 0; i < particleCount; i++)
	{
		float particleInitPosX = (rand() / (float)RAND_MAX - 0.5) * 2;
		float particleInitPosY = (rand() / (float)RAND_MAX - 0.5) * 2;
		float randomValueX = 0.f;
		float randomValueY = 0.f;
		float randomValueZ = 0.f;
		float randomStartTime = 0.f;
		float velocityScale = 0.5f;

		randomValueX = (rand() / (float)RAND_MAX - 0.5)*velocityScale;
		randomValueY = (rand() / (float)RAND_MAX - 0.5)*velocityScale;
		randomValueZ = 0.f;
		randomStartTime = (rand() / (float)RAND_MAX)*20.f;

		particleVertices[particleVertIndex] = -particleSize / 2.f + particleInitPosX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = -particleSize / 2.f + particleInitPosY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueZ;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomStartTime;
		particleVertIndex++;

		particleVertices[particleVertIndex] = particleSize / 2.f + particleInitPosX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = -particleSize / 2.f + particleInitPosY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 1.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueZ;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomStartTime;
		particleVertIndex++;

		particleVertices[particleVertIndex] = particleSize / 2.f + particleInitPosX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = particleSize / 2.f + particleInitPosY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 1.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 1.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueZ;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomStartTime;
		particleVertIndex++;

		particleVertices[particleVertIndex] = -particleSize / 2.f + particleInitPosX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = -particleSize / 2.f + particleInitPosY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueZ;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomStartTime;
		particleVertIndex++;

		particleVertices[particleVertIndex] = particleSize / 2.f + particleInitPosX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = particleSize / 2.f + particleInitPosY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 1.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 1.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueZ;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomStartTime;
		particleVertIndex++;

		particleVertices[particleVertIndex] = -particleSize / 2.f + particleInitPosX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = particleSize / 2.f + particleInitPosY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 0.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = 1.f;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueX;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueY;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomValueZ;
		particleVertIndex++;
		particleVertices[particleVertIndex] = randomStartTime;
		particleVertIndex++;
	}

	glGenBuffers(1, &m_VBOVertexTextureVelocityParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertexTextureVelocityParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*particleFloatCount, particleVertices, GL_STATIC_DRAW);
}

void Renderer::CreateParticleVertex()
{
	m_vertexParticleCount = 100;

	float *vertexParticles = new float[m_vertexParticleCount * 4];

	for (int i = 0; i < m_vertexParticleCount; ++i)
	{
		vertexParticles[i * 4] = (i / (float)(m_vertexParticleCount - 1)) * 2 - 1;
		vertexParticles[i * 4 + 1] = i / 100;
		vertexParticles[i * 4 + 2] = i / 100;
		vertexParticles[i * 4 + 3] = (rand() / (float)RAND_MAX) * 2 - 1;
	}

	glGenBuffers(1, &m_VBOVertexParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertexParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexParticleCount * 4, vertexParticles, GL_STATIC_DRAW);
}

void Renderer::DrawVertexParticleAnim(float timeElapsed)
{
	glEnable(GL_CULL_FACE);
	GLuint shader{ m_VertexParticleVer2Shader };

	glUseProgram(shader);

	m_Time += timeElapsed;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TEXParticlePostion);

	GLuint gTimeId = glGetUniformLocation(shader, "u_Time");
	GLuint gPeriodId = glGetUniformLocation(shader, "u_Period");
	GLuint gHeight = glGetUniformLocation(shader, "u_Height");
	GLuint uProjViewId = glGetUniformLocation(shader, "u_ProjView");
	GLuint gTextureId = glGetUniformLocation(shader, "uSampler2D");

	glUniform1f(gTimeId, m_Time);
	glUniform1f(gPeriodId, 8);
	glUniform1f(gHeight, 0.5);

	glUniformMatrix4fv(uProjViewId, 1, GL_FALSE, &m_Camera->GetProjViewMat()[0][0]);
	glUniform1i(gTextureId, 0);

	GLuint posId = glGetAttribLocation(shader, "a_Position");

	glEnableVertexAttribArray(posId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertexParticle);

	glVertexAttribPointer(posId, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glDrawArrays(GL_POINTS, 0, m_vertexParticleCount);

	glDisableVertexAttribArray(posId);
	glDisable(GL_CULL_FACE);
}

void Renderer::DrawProxyGeometry()
{
	GLuint shader{ m_ProxyGeometryShader };

	glUseProgram(shader);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint uProjViewId = glGetUniformLocation(shader, "u_ProjView");
	glUniformMatrix4fv(uProjViewId, 1, GL_FALSE, &m_Camera->GetProjViewMat()[0][0]);

	GLuint posId = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posId);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOProxyGeometry);

	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_LINE_STRIP, 0, m_ProxyGeometryCount);
	glDisableVertexAttribArray(posId);
	glDisable(GL_DEPTH_TEST);
}

void Renderer::DrawQuad()
{
	GLuint shader{ m_QuadShader };

	glUseProgram(shader);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint uProjViewId = glGetUniformLocation(shader, "u_ProjView");

	glUniformMatrix4fv(uProjViewId, 1, GL_FALSE, &m_Camera->GetProjViewMat()[0][0]);

	GLuint posId = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posId);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOQuad);

	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_LINE_STRIP, 0, 6);
	glDisableVertexAttribArray(posId);
	
	glDisable(GL_DEPTH_TEST);
}

void Renderer::DrawVertexTextureParticleAnim(float timeElapsed)
{
	GLuint shader = m_VertexTextureParticleShader;

	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Time += timeElapsed;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TEXParticle);

	GLuint uProjViewId = glGetUniformLocation(shader, "u_ProjView");
	glUniformMatrix4fv(uProjViewId, 1, GL_FALSE, &m_Camera->GetProjViewMat()[0][0]);

	GLuint gTextureId = glGetUniformLocation(shader, "u_Texture");
	glUniform1i(gTextureId, 0);

	GLuint uniformTime = glGetUniformLocation(shader, "u_Time");

	glUniform1f(uniformTime, m_Time);

	GLuint attrribPosition = glGetAttribLocation(shader, "a_Position");
	GLuint attrribTexPos = glGetAttribLocation(shader, "a_TexPos");
	GLuint attrribVelocity = glGetAttribLocation(shader, "a_Velocity");

	glEnableVertexAttribArray(attrribPosition);
	glEnableVertexAttribArray(attrribTexPos);
	glEnableVertexAttribArray(attrribVelocity);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertexTextureVelocityParticle);
	glVertexAttribPointer(attrribPosition, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
	glVertexAttribPointer(attrribTexPos, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glVertexAttribPointer(attrribVelocity, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(5 * sizeof(float)));

	glDrawArrays(GL_TRIANGLES, 0, m_vertexTextureVelocityParticleCount);

	glDisableVertexAttribArray(attrribPosition);
	glDisableVertexAttribArray(attrribTexPos);
	glDisableVertexAttribArray(attrribVelocity);

	glDisable(GL_BLEND);
}

