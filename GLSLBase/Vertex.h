#pragma once
class Vertex
{
private:
	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec4 mTexcoord0;
	glm::vec4 mVertexColor;
public:
	Vertex();
	Vertex(glm::vec3 _Position);
	Vertex(glm::vec3 _Position, glm::vec3 _Normal);
	Vertex(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0);
	Vertex(glm::vec3 _Position, glm::vec3 _Normal, glm::vec4 _Texcoord0, glm::vec4 _VertexColor);
	Vertex(const Vertex& _Vertex);
	Vertex(Vertex&& _Vertex);
	~Vertex();
	
	
	Vertex& operator= (const Vertex& _Vertex)
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
	
	void SetNormal(glm::vec3 _Normal) { mNormal = _Normal; }

private:

};
