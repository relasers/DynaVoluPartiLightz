#include "stdafx.h"
#include "Vertex.h"

Vertex::Vertex()
{
}

Vertex::Vertex(glm::vec3 _Position) : mPosition(_Position) , mNormal(glm::vec3(0,0,0)), mTexcoord0(glm::vec4(0,0,0,0)), mVertexColor(glm::vec4(0, 0, 0, 0))
{
}

Vertex::Vertex(glm::vec3 _Position, glm::vec3 _Normal) : mPosition(_Position), mNormal(_Normal), mTexcoord0(glm::vec4(0, 0, 0, 0)), mVertexColor(glm::vec4(0, 0, 0, 0))
{
}

Vertex::Vertex(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0) : mPosition(_Position), mNormal(_Normal), mTexcoord0(_Texcoord0), mVertexColor(glm::vec4(0, 0, 0, 0))
{
}

Vertex::Vertex(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0, glm::vec4 _VertexColor) : mPosition(_Position), mNormal(_Normal), mTexcoord0(_Texcoord0), mVertexColor(_VertexColor)
{
}

Vertex::Vertex(const Vertex & _Vertex): mPosition(_Vertex.mPosition), mNormal(_Vertex.mNormal), mTexcoord0(_Vertex.mTexcoord0), mVertexColor(_Vertex.mVertexColor)
{
}

Vertex::Vertex(Vertex && _Vertex) : mPosition(_Vertex.mPosition), mNormal(_Vertex.mNormal), mTexcoord0(_Vertex.mTexcoord0), mVertexColor(_Vertex.mVertexColor)
{
}

Vertex::~Vertex()
{
}
