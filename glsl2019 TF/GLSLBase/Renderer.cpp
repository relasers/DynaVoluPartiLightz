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

void Renderer::CreateTFVBO()
{
	float data[] = { -0.5f, 0.5f, 0.5f };
	//float data[] = { 1.0f, 2.0f, 3.0f };

	// vbo
	glGenBuffers(1, &m_TFVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TFVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	// tbo
	glGenBuffers(1, &m_TFTBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TFTBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);
}

void Renderer::CreateTFwithGSVBO()
{
	vertexData data[1];

	data[0].pos = glm::vec4(-0.5f, 0.5f, 0.5f, 0.5f);
	data[0].uv = glm::vec2(0.0f, 0.0f);
	data[0].color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	// tbo
	glGenTransformFeedbacks(2, m_TFwithGSTBO);
	glGenBuffers(2, m_TFwithCopyTBO);

	for (unsigned int i = 0; i < 2 ; i++) {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TFwithGSTBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_TFwithCopyTBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TFwithCopyTBO[i]);
    }
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	
	// Attach까지만 진행
	m_TFwithGSShader = CompileShadersNoLink("./Shaders/TFWithGSVertexShader.glvs", "./Shaders/TFWithGSGeometryShader.glgs", "./Shaders/TFWithGSFragmentShader.glfs");

	//Create VBOs
	CreateVertexBufferObjects();
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

	CreateTFVBO();
	CreateTFwithGSVBO();
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
GLuint Renderer::CompileShadersNoLink(char * filenameVS, char * filenameFS)
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

	return ShaderProgram;
}

GLuint Renderer::CompileShadersNoLink(char * filenameVS, char * filenameGS, char * filenameFS)
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

	// shader.glgs 가 gs 안으로 로딩
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

	// ShaderProgram 에 gs.c_str() 기하 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, gs.c_str(), GL_GEOMETRY_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

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
	glUseProgram(m_SolidRectShader);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);
}


void Renderer::TransformFeedbackFunc()
{
	// Attach까지만 진행
	m_TFShader = CompileShadersNoLink("./Shaders/TFVertexShader.glvs", "./Shaders/TFFragmentShader.glfs");

	// transform feedback
	const GLchar* feedbackVaryings[] = { "outValue" };
	glTransformFeedbackVaryings(m_TFShader, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);	

	// Link 진행
	glLinkProgram(m_TFShader);

	// Use
	glUseProgram(m_TFShader);

	GLint attribPosition = glGetAttribLocation(m_TFShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_TFVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//glEnable(GL_RASTERIZER_DISCARD);

	// glBindBufferBase
	// tbo 버퍼를 피드백 버퍼로 실제로 바인드 하기 위한 함수
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TFTBO);

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 1);

	glEndTransformFeedback();
	//glFlush();
	//glDisable(GL_RASTERIZER_DISCARD);

	glPointSize(5);

	GLfloat feedback[1];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

	printf("%f \n", feedback[0]);
}

void Renderer::TransformFeedbackWithGSFunc()
{
	GLuint shader = m_TFwithGSShader;

	// transform feedback
	const GLchar* feedbackVaryings[] = { "outPos", "outUV", "outColor" };
	glTransformFeedbackVaryings(shader, 3, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	// Link 진행
	glLinkProgram(shader);

	// Use
	glUseProgram(shader);

	glEnable(GL_RASTERIZER_DISCARD);

	// attribute or uniform
	GLint attribPosition = glGetAttribLocation(shader, "a_Position");
	GLint aUV = glGetAttribLocation(shader, "a_UV");
	GLint aColor = glGetAttribLocation(shader, "a_Color");
	
	glBindBuffer(GL_ARRAY_BUFFER, m_TFwithCopyTBO[m_Num]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TFwithGSTBO[m_NumTFB]);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(aUV);
	glEnableVertexAttribArray(aColor);

	glVertexAttribPointer(attribPosition, 4, GL_FLOAT, GL_FALSE, sizeof(vertexData), 0);
	glVertexAttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(vertexData), (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertexData), (GLvoid*)(sizeof(float) * 6));

	// Rasterazer
	glBeginTransformFeedback(GL_POINTS);
	
	if (m_isFirst) {
		glDrawArrays(GL_POINTS, 0, 1);
		m_isFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, m_TFwithGSTBO[m_Num]);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(aUV);
	glDisableVertexAttribArray(aColor);

	//vertexData feedback[1];
	//glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
	//
	//for (int i = 0; i < 1; ++i)
	//{
	//	printf("Vertex Pos   %d : %.2f, %.2f, %.2f, %.2f \n", i, feedback[i].pos.x, feedback[i].pos.y, feedback[i].pos.z, feedback[i].pos.w);
	//	printf("Vertex UV    %d : %.2f, %.2f \n", i, feedback[i].uv.x, feedback[i].uv.y);
	//	printf("Vertex Color %d : %.2f, %.2f, %.2f, %.2f \n", i, feedback[i].color.x, feedback[i].color.y, feedback[i].color.z, feedback[i].color.w);
	//}
}

void Renderer::TBORenderingTest()
{
	GLuint shader = m_TFwithGSShader;

	/*
	// Attach까지만 진행
	m_TFwithGSShader = CompileShadersNoLink("./Shaders/RenderToTBO.glvs", "./Shaders/RenderToTBO.glgs", "./Shaders/RenderToTBO.glfs");

	// transform feedback
	const GLchar* feedbackVaryings[] = { "outPos", "outUV", "outColor" };
	glTransformFeedbackVaryings(m_TFwithGSShader, 3, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	// Link 진행
	glLinkProgram(m_TFwithTBOshader);

	GLuint shader = m_TFwithTBOshader;

	glUseProgram(shader);

	GLint attribPosition = glGetAttribLocation(shader, "a_Position");
	GLint aUV = glGetAttribLocation(shader, "a_UV");
	GLint aColor = glGetAttribLocation(shader, "a_Color");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(aUV);
	glEnableVertexAttribArray(aColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_TFwithGSTBO[m_Num % 2]);

	glVertexAttribPointer(attribPosition, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
	glVertexAttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 6));

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_TFwithGSTBO[++m_Num % 2]);

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 1);
	glEndTransformFeedback();
	*/
	glDisable(GL_RASTERIZER_DISCARD);

	GLint attribPosition = glGetAttribLocation(shader, "a_Position");
	GLint aUV = glGetAttribLocation(shader, "a_UV");
	GLint aColor = glGetAttribLocation(shader, "a_Color");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(aUV);
	glEnableVertexAttribArray(aColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_TFwithCopyTBO[m_NumTFB]);

	glVertexAttribPointer(attribPosition, 4, GL_FLOAT, GL_FALSE, sizeof(vertexData), 0);
	glVertexAttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(vertexData), (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(vertexData), (GLvoid*)(sizeof(float) * 6));

	glDrawTransformFeedback(GL_POINTS, m_TFwithGSTBO[m_NumTFB]);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(aUV);
	glDisableVertexAttribArray(aColor);

	m_Num = m_NumTFB;
	m_NumTFB = (m_NumTFB + 1) & 0x1;

}
