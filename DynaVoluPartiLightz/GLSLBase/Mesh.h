#pragma once
#include"Vertex.h"
class Mesh
{
protected:
	std::string mName;
	std::vector<Vertex> mVertices;

	std::vector<float> mVerticesData;
	std::vector<unsigned int> mIndices;
	
public:
	Mesh();
	virtual ~Mesh();

	virtual bool BuildGeomData() { return false; };

	void* GetVerticesAdress()
	{
		return &mVertices[0];
	}

	void* GetIndicesAdress()
	{
		return &mIndices[0];
	}

	std::vector<Vertex> GetVerticesVector() { return mVertices; }
	std::vector<float> GetVerticesData() { return mVerticesData; }
	std::vector<unsigned int> GetIndicesVector() { return mIndices; }

};

class PlaneMesh : public Mesh
{
public:

	PlaneMesh();
	virtual ~PlaneMesh();
	virtual bool BuildGeomData() override;
	/// divide level : 0 --> Quad 
	virtual bool BuildGeomData(float size, float divide_level);



};