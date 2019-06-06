#include "stdafx.h"
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram()
	: mHandle(UINT32_MAX)
{
}

ShaderProgram::~ShaderProgram()
{
	ResetProgram();
}

void ShaderProgram::compile(const std::string& file_path_vs, const std::string& file_path_fs)
{
	ResetProgram();

	mHandle = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (GL_FALSE == mHandle) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (false == ReadFile(vs, file_path_vs)) {
		printf("Error compiling vertex shader\n");
		return;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (false == ReadFile(fs, file_path_fs)) {
		printf("Error compiling fragment shader\n");
		return;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(mHandle, vs, GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(mHandle, fs, GL_FRAGMENT_SHADER);

	GLint success = GL_FALSE;
	GLchar ErrorLog[LOG_SIZE] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(mHandle);

	//링크가 성공했는지 확인
	glGetProgramiv(mHandle, GL_LINK_STATUS, &success);

	if (GL_FALSE == success) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(mHandle, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << file_path_vs << ", " << file_path_fs << " Error linking shader program\n" << ErrorLog;
		return;
	}

	glValidateProgram(mHandle);
	glGetProgramiv(mHandle, GL_VALIDATE_STATUS, &success);
	if (GL_FALSE == success) {
		glGetProgramInfoLog(mHandle, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << file_path_vs << ", " << file_path_fs << " Error validating shader program\n" << ErrorLog;
		return;
	}

	glUseProgram(mHandle);
	std::cout << file_path_vs << ", " << file_path_fs << " Shader compiling is done.\n";
}

void ShaderProgram::compile(const std::string& file_path_vs, const std::string& file_path_gs, const std::string& file_path_fs)
{
	ResetProgram();

	mHandle = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (GL_FALSE == mHandle) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, gs, fs;

	if (false == ReadFile(vs, file_path_vs)) {
		printf("Error compiling vertex shader\n");
		return;
	};

	if (false == ReadFile(gs, file_path_gs)) {
		printf("Error compiling geometry shader\n");
		return;
	};

	if (false == ReadFile(fs, file_path_fs)) {
		printf("Error compiling fragment shader\n");
		return;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(mHandle, vs, GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 기하 쉐이더를 컴파일한 결과를 attach함
	AddShader(mHandle, gs, GL_GEOMETRY_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(mHandle, fs, GL_FRAGMENT_SHADER);

	GLint success = GL_FALSE;
	GLchar ErrorLog[LOG_SIZE] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(mHandle);

	//링크가 성공했는지 확인
	glGetProgramiv(mHandle, GL_LINK_STATUS, &success);

	if (GL_FALSE == success) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(mHandle, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << file_path_vs << ", " << file_path_fs << " Error linking shader program\n" << ErrorLog;
		return;
	}

	glValidateProgram(mHandle);
	glGetProgramiv(mHandle, GL_VALIDATE_STATUS, &success);
	if (GL_FALSE == success) {
		glGetProgramInfoLog(mHandle, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << file_path_vs << ", " << file_path_fs << " Error validating shader program\n" << ErrorLog;
		return;
	}

	glUseProgram(mHandle);
	std::cout << file_path_vs << ", " << file_path_fs << " Shader compiling is done.\n";
}

bool ShaderProgram::ReadFile(std::string& out_data, const std::string& filename)
{
	std::ifstream file(filename);

	if (true == file.fail()) {
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}

	std::string line;
	while (getline(file, line)) {
		out_data.append(line.c_str());
		out_data.append("\n");
	}
	return true;
}

void ShaderProgram::AddShader(GLuint ShaderProgram, const std::string& ShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (GL_FALSE == ShaderObj) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p = ShaderText.c_str();
	const GLint Lengths = static_cast<GLint>(ShaderText.length());

	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, &p, &Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	GLint success = GL_FALSE;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (GL_FALSE == success) {
		GLchar InfoLog[LOG_SIZE];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, LOG_SIZE, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", ShaderText.c_str());
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

void ShaderProgram::ResetProgram()
{
	if (GL_TRUE == glIsProgram(mHandle))
		glDeleteProgram(mHandle);
}
