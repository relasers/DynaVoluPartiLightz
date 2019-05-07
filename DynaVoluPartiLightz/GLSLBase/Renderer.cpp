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
	


	//Create Geometry Data
	CreateGeometryDataMeshes();
	//Create VBOs
	CreateVertexBufferObjects();

	 m_Texture["OldPage"] = CreatePngTexture("./Resource/Texture/OldPage.png");
	 m_Texture["BackPage"] = CreatePngTexture("./Resource/Texture/BackPage.png");
	 m_Texture["Leather"] = CreatePngTexture("./Resource/Texture/Leather.png");

}

void Renderer::CreateGeometryDataMeshes()
{
	mPlaneMesh.BuildGeomData(50,50);
	
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

	
	CreateVBOandIBOfromPlaneMesh(mPlaneMesh);
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
	m_Texture["TexP1"] = 0;
	m_Texture["TexP1Depth"] = 0;
	m_FBO["FrameBufferP1"] = 0;

	glGenTextures(1, &m_Texture["TexP1"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["TexP1"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// 이때는 메모리 할당만 시행한다.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &m_Texture["TexP1Depth"]);
	glBindTexture(GL_TEXTURE_2D, m_Texture["TexP1Depth"]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// 깊이 버퍼를 위한 포맷
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_WindowSizeX, m_WindowSizeY, 0, GL_DEPTH_COMPONENT24, GL_FLOAT, 0);

	glGenFramebuffers(1, &m_FBO["FrameBufferP1"]);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO["FrameBufferP1"]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture["TexP1"], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_Texture["TexP1Depth"], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::CreateVBOandIBOfromPlaneMesh(PlaneMesh& mesh)
{

	GLuint VBO;
	GLuint IBO;



	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVerticesData().size() * sizeof(float), &mesh.GetVerticesData()[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndicesVector().size() * sizeof(unsigned int), &mesh.GetIndicesVector()[0], GL_STATIC_DRAW);

	m_VBO["Plane"] = VBO;
	m_IBO["Plane"] = IBO;

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
	glUniformMatrix4fv(model, 1, GL_FALSE, &mPlaneTransform.GetModelMatrix()[0][0]);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 10));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLE_STRIP, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);
	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);
}

void Renderer::DrawSolidMesh()
{
	GLuint shader = m_Shaders["SolidMesh"];
	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mPlaneTransform.GetModelMatrix()[0][0]);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 10));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
	glDrawElements(GL_TRIANGLES, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);

	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);
}

void Renderer::PageFlipCase01(float _theta)
{
	GLuint shader = m_Shaders["PageFlipBasic"];
	glUseProgram(shader);


	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");
	GLuint theta = glGetUniformLocation(shader,"u_Theta");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mPlaneTransform.GetModelMatrix()[0][0]);
	glUniform1f(theta,_theta );

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0);
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 7));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLE_STRIP, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);
	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);
}

void Renderer::PageFlipCase01Tex(float _theta, glm::vec2 _PivotPos, float _PivotLineTheta)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint shader = m_Shaders["PageFlipBasicTex"];
	glUseProgram(shader);

	GLuint gTextureFront = glGetUniformLocation(shader, "u_TextureFront");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture["OldPage"]);
	// f 쓰면 망함 
	glUniform1i(gTextureFront, 0);
	
	GLuint gTextureBack = glGetUniformLocation(shader, "u_TextureBack");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Texture["BackPage"]);
	glUniform1i(gTextureBack, 1);

	GLuint gLinePivot = glGetUniformLocation(shader, "u_LinePivot");
	GLuint gLinePivotTheta = glGetUniformLocation(shader, "u_FlipTheta");

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");
	GLuint theta = glGetUniformLocation(shader, "u_Theta");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mPlaneTransform.GetModelMatrix()[0][0]);
	glUniform1f(theta, _theta);
	
	glUniform2f(gLinePivot, _PivotPos.x, _PivotPos.y);
	glUniform1f(gLinePivotTheta, _PivotLineTheta);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0);
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 7));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);
	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);

	glDisable(GL_DEPTH_TEST);
}

void Renderer::PageFlipCase02(float _delta, glm::vec2 _PivotPos, float _PivotLineTheta)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint shader = m_Shaders["PageFlipRoller"];
	glUseProgram(shader);

	GLuint gTextureFront = glGetUniformLocation(shader, "u_TextureFront");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture["OldPage"]);
	// f 쓰면 망함 
	glUniform1i(gTextureFront, 0);

	GLuint gTextureBack = glGetUniformLocation(shader, "u_TextureBack");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Texture["BackPage"]);
	glUniform1i(gTextureBack, 1);

	GLuint gAxisPivot = glGetUniformLocation(shader, "u_AxisPivot");
	GLuint gLinePivotTheta = glGetUniformLocation(shader, "u_FlipTheta");
	GLuint gLinePaperRadius = glGetUniformLocation(shader, "u_PaperRadius");

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");
	GLuint delta = glGetUniformLocation(shader, "u_Delta");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mPlaneTransform.GetModelMatrix()[0][0]);
	glUniform1f(delta, _delta);



	glUniform2f(gAxisPivot, _PivotPos.x, _PivotPos.y);
	glUniform1f(gLinePivotTheta, _PivotLineTheta);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0);
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 7));
	glUniform1f(gLinePaperRadius,1.0f);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);
	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);

	glDisable(GL_DEPTH_TEST);
}

void Renderer::PageFlipCase02_temp(float _delta, glm::vec2 _PivotPos, float _PivotLineTheta)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint shader = m_Shaders["PageFlipRollerTemp"];
	glUseProgram(shader);

	GLuint gTextureFront = glGetUniformLocation(shader, "u_TextureFront");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture["OldPage"]);
	// f 쓰면 망함 
	glUniform1i(gTextureFront, 0);

	GLuint gTextureBack = glGetUniformLocation(shader, "u_TextureBack");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Texture["BackPage"]);
	glUniform1i(gTextureBack, 1);

	GLuint gAxisPivot = glGetUniformLocation(shader, "u_AxisPivot");
	GLuint gLinePivotTheta = glGetUniformLocation(shader, "u_FlipTheta");
	GLuint gLinePaperRadius = glGetUniformLocation(shader, "u_PaperRadius");

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint model = glGetUniformLocation(shader, "u_Model");
	GLuint delta = glGetUniformLocation(shader, "u_Delta");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &mCamera.GetProjViewMatrix()[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &mPlaneTransform.GetModelMatrix()[0][0]);
	glUniform1f(delta, _delta);



	glUniform2f(gAxisPivot, _PivotPos.x, _PivotPos.y);
	glUniform1f(gLinePivotTheta, _PivotLineTheta);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO["Plane"]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0);
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (GLvoid*)(sizeof(float) * 7));
	glUniform1f(gLinePaperRadius, 1.0f);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO["Plane"]);
	glLineWidth(2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, mPlaneMesh.GetIndicesVector().size(), GL_UNSIGNED_INT, 0);
	//std::cout << "|" << m_VBO["Plane"] << " " << m_IBO["Plane"] << std::endl;

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);

	glDisable(GL_DEPTH_TEST);

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

void Renderer::SetFrameBuffer()
{
	GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);


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
	mCamera.UpdateCameraMatrix();
}

void Renderer::CameraMove(glm::vec3 _velocity, float delta)
{
	glm::vec3 newPosition = mCamera.GetCameraPos() + _velocity * delta;
	mCamera.SetCameraPosition(newPosition);
}
