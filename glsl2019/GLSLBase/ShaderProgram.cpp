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

	mHandle = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (GL_FALSE == mHandle) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (false == ReadFile(vs, file_path_vs)) {
		printf("Error compiling vertex shader\n");
		return;
	};

	//shader.fs �� fs ������ �ε���
	if (false == ReadFile(fs, file_path_fs)) {
		printf("Error compiling fragment shader\n");
		return;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(mHandle, vs, GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(mHandle, fs, GL_FRAGMENT_SHADER);

	GLint success = GL_FALSE;
	GLchar ErrorLog[LOG_SIZE] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(mHandle);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(mHandle, GL_LINK_STATUS, &success);

	if (GL_FALSE == success) {
		// shader program �α׸� �޾ƿ�
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

	mHandle = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (GL_FALSE == mHandle) { //���̴� ���α׷��� ����������� Ȯ��
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

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(mHandle, vs, GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() ���� ���̴��� �������� ����� attach��
	AddShader(mHandle, gs, GL_GEOMETRY_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(mHandle, fs, GL_FRAGMENT_SHADER);

	GLint success = GL_FALSE;
	GLchar ErrorLog[LOG_SIZE] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(mHandle);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(mHandle, GL_LINK_STATUS, &success);

	if (GL_FALSE == success) {
		// shader program �α׸� �޾ƿ�
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
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (GL_FALSE == ShaderObj) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p = ShaderText.c_str();
	const GLint Lengths = static_cast<GLint>(ShaderText.length());

	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, &p, &Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	GLint success = GL_FALSE;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (GL_FALSE == success) {
		GLchar InfoLog[LOG_SIZE];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, LOG_SIZE, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", ShaderText.c_str());
	}

	// ShaderProgram �� attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

void ShaderProgram::ResetProgram()
{
	if (GL_TRUE == glIsProgram(mHandle))
		glDeleteProgram(mHandle);
}
