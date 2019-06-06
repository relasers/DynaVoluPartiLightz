#include "stdafx.h"
#include "Vertex.h"

Vertex::Vertex()
{
	memset(f, 0, sizeof(Vertex));
}

float& Vertex::operator[](int idx)
{
	return f[idx];
}

Vertex& Vertex::operator=(const Vertex& other)
{
	memcpy_s(f, sizeof(Vertex), other.f, sizeof(Vertex));
	return *this;
}
