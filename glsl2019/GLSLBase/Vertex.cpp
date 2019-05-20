#include "stdafx.h"
#include "Vertex.h"

Vertex::Vertex()
{
	pos = { RAND_FLOAT(-0.5f, 0.5f), RAND_FLOAT(-0.5f, 0.5f), RAND_FLOAT(-0.5f, 0.5f) };
	dir = glm::normalize(glm::vec3{ RAND_FLOAT(-0.5f, 0.5f), RAND_FLOAT(-0.5f, 0.5f), RAND_FLOAT(-0.5f, 0.5f) });
	speed = RAND_FLOAT(0.01f, 0.05f);
	collide_time = 0.0f;
}

float& Vertex::operator[](int idx)
{
	return f[idx];
}
