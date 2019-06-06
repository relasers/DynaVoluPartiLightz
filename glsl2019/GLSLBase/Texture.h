#pragma once

class Texture2D {
public:
	Texture2D();
	~Texture2D();

	void load(const std::string& file_path, GLint min_mag_filter = GL_NEAREST);
	void load(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void* init_datas = nullptr, GLint min_mag_filter = GL_NEAREST);

	void bind();

	GLuint get() const { return mHandle; }

private:
	void loadBMPRaw(const std::string& filePath, std::vector<unsigned char>& outImage, unsigned int& outWidth, unsigned int& outHeight);
	GLuint CreateBmpTexture(const std::string& filePath, GLint min_mag_filter);
	GLuint CreatePngTexture(const std::string& filePath, GLint min_mag_filter);

private:
	GLuint mHandle;
};