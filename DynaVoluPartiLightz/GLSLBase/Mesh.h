#pragma once
#include"Vertex.h"
class Mesh
{
protected:
	std::string mName;
	std::vector<Vertex> mVertices;

	//std::vector<float> mVerticesData;
	std::vector<unsigned int> mIndices;

	unsigned int mNumVertices;
	
public:
	Mesh();
	Mesh(std::string _name , std::string path);
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
	//std::vector<float> GetVerticesData() { return mVerticesData; }
	std::vector<unsigned int> GetIndicesVector() { return mIndices; }
	std::string GetName() const { return mName; }
	unsigned int GetNumVertices() const { return mNumVertices; }


	bool loadOBJ(std::string path);
	void InitLoadMesh(const aiMesh * pMesh);
	void UpdateVerticesDataForVBO();


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