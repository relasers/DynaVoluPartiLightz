#include "stdafx.h"
#include "Texture.h"
#include "LoadPng.h"

Texture2D::Texture2D() : mHandle(UINT32_MAX)
{
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mHandle);
}

void Texture2D::load(const std::string& file_path, GLint min_mag_filter)
{
	int offset = static_cast<int>(file_path.size()) - 1 - 3;
	std::string format = static_cast<const char*>(file_path.data() + offset);
	if (format == "png" || format == "PNG") {
		mHandle = CreatePngTexture(file_path, min_mag_filter);
		return;
	}
	if (format == "bmp" || format == "BMP") {
		mHandle = CreateBmpTexture(file_path, min_mag_filter);
		return;
	}
}

void Texture2D::load(GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum data_type, const void* init_datas, GLint min_mag_filter)
{
	glGenTextures(1, &mHandle);
	glBindTexture(GL_TEXTURE_2D, mHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, data_type, init_datas);
}

void Texture2D::bind()
{
	glBindTexture(GL_TEXTURE_2D, mHandle);
}

void Texture2D::loadBMPRaw(const std::string& filePath, std::vector<unsigned char>& outImage, unsigned int& outWidth, unsigned int& outHeight)
{
	std::cout << "Loading bmp file " << filePath << " ... " << std::endl;
	outWidth = -1;
	outHeight = -1;

	// Data read from the header of the BMP file
	const unsigned int header_size = 54;
	unsigned char header[header_size];
	unsigned int dataPos = 0;
	unsigned int imageSize = 0;

	// Actual RGB data
	unsigned char* data = nullptr;

	// Open the file
	FILE* file = nullptr;
	fopen_s(&file, filePath.c_str(), "rb");
	if (nullptr == file) {
		std::cout << "Image could not be opened, " << filePath << " is missing. " << std::endl;
		return;
	}

	if (54 != fread(header, 1, header_size, file)) {
		std::cout << filePath << " is not a correct BMP file. " << std::endl;
		return;
	}

	if ('B' != header[0] || 'M' != header[1]) {
		std::cout << filePath << " is not a correct BMP file. " << std::endl;
		return;
	}

	if (0 != (*reinterpret_cast<int*>(&(header[0x1E])))) {
		std::cout << filePath << " is not a correct BMP file. " << std::endl;
		return;
	}

	if (24 != (*reinterpret_cast<int*>(&(header[0x1C])))) {
		std::cout << filePath << " is not a correct BMP file. " << std::endl;
		return;
	}

	dataPos = *reinterpret_cast<int*>(&(header[0x0A]));
	imageSize = *reinterpret_cast<int*>(&(header[0x22]));
	outWidth = *reinterpret_cast<int*>(&(header[0x12]));
	outHeight = *reinterpret_cast<int*>(&(header[0x16]));

	if (0 == imageSize) imageSize = outWidth * outHeight * 3;
	if (0 == dataPos) dataPos = header_size;

	outImage.resize(imageSize);
	fread(outImage.data(), 1, imageSize, file);
	fclose(file);

	std::cout << filePath << " is succesfully loaded. " << std::endl;
}

GLuint Texture2D::CreateBmpTexture(const std::string& filePath, GLint min_mag_filter)
{
	//Load Bmp: Load file and decode image.
	unsigned int width = 0, height = 0;
	std::vector<unsigned char> bmp;
	loadBMPRaw(filePath, bmp, width, height);

	GLuint temp = -1;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.data());

	return temp;
}

GLuint Texture2D::CreatePngTexture(const std::string& filePath, GLint min_mag_filter)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned int width = 0, height = 0;
	unsigned int error = lodepng::decode(image, width, height, filePath);

	if (0 != error) {
		lodepng_error_text(error);
		assert(0 == error);
		return -1;
	}

	GLuint temp = -1;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	return temp;
}
