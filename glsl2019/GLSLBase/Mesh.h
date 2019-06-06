#pragma once
#include "Vertex.h"

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	bool load(const std::string& filepath);
	void bind();
	void render();

protected:
	std::vector<Vertex>			mVertices;
	std::vector<unsigned int>	mIndices;
	GLuint						mVBO;
	GLuint						mIBO;
};