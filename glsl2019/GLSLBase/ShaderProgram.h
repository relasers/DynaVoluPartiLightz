#pragma once

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void compile(const std::string& file_path_vs, const std::string& file_path_fs);
	void compile(const std::string& file_path_vs, const std::string& file_path_gs, const std::string& file_path_fs);

	GLuint get() const { return mHandle; }

private:
	bool ReadFile(std::string& out_data, const std::string& filename);
	void AddShader(GLuint ShaderProgram, const std::string& ShaderText, GLenum ShaderType);

	void ResetProgram();

private:
	GLuint mHandle;
};