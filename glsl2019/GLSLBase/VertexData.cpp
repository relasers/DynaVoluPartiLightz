#include "stdafx.h"
#include "VertexData.h"

VertexData::VertexData()
{
}

VertexData::VertexData(glm::vec3 _Position) : mPosition(_Position) , mNormal(glm::vec3(0,0,0)), mTexcoord0(glm::vec4(0,0,0,0)), mVertexColor(glm::vec4(0, 0, 0, 0))
{				  
}				  
				  
VertexData::VertexData(glm::vec3 _Position, glm::vec3 _Normal) : mPosition(_Position), mNormal(_Normal), mTexcoord0(glm::vec4(0, 0, 0, 0)), mVertexColor(glm::vec4(0, 0, 0, 0))
{				  
}				  
				  
VertexData::VertexData(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0) : mPosition(_Position), mNormal(_Normal), mTexcoord0(_Texcoord0), mVertexColor(glm::vec4(0, 0, 0, 0))
{				  
}				  
				  
VertexData::VertexData(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0, glm::vec4 _VertexColor) : mPosition(_Position), mNormal(_Normal), mTexcoord0(_Texcoord0), mVertexColor(_VertexColor)
{
}

VertexData::VertexData(const VertexData & _Vertex): mPosition(_Vertex.mPosition), mNormal(_Vertex.mNormal), mTexcoord0(_Vertex.mTexcoord0), mVertexColor(_Vertex.mVertexColor)
{
}

VertexData::VertexData(VertexData && _Vertex) : mPosition(_Vertex.mPosition), mNormal(_Vertex.mNormal), mTexcoord0(_Vertex.mTexcoord0), mVertexColor(_Vertex.mVertexColor)
{
}

VertexData::~VertexData()
{
}
