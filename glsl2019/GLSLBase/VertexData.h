#pragma once
class VertexData
{
public:
	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec4 mTexcoord0;
	glm::vec4 mVertexColor;
public:
	VertexData();
	VertexData(glm::vec3 _Position);
	VertexData(glm::vec3 _Position, glm::vec3 _Normal);
	VertexData(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0);
	VertexData(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0, glm::vec4 _VertexColor);
	VertexData(const VertexData& _Vertex);
	VertexData(VertexData&& _Vertex);
	~VertexData();
	
	
	VertexData& operator= (const VertexData& _Vertex)
	{
		mPosition = _Vertex.mPosition;
		mNormal = _Vertex.mNormal;
		mTexcoord0 = _Vertex.mTexcoord0;
		mVertexColor = _Vertex.mVertexColor;
		return *this;
	}

	glm::vec3 GetPosition(){ return mPosition; }
	glm::vec3 GetNormal() { return mNormal; }
	glm::vec4 GetTexCoord() { return mTexcoord0; }
	glm::vec4 GetVertexColor() { return mVertexColor; }
	
	void SetPosition(glm::vec3 _Position) { mPosition = _Position; }
	void SetTexCoord0(glm::vec4 _TexCoord0) { mTexcoord0 = _TexCoord0; }
	void SetNormal(glm::vec3 _Normal) { mNormal = _Normal; }
	void SetVertexColor(glm::vec4 _Color) { mVertexColor = _Color; }

private:

};
