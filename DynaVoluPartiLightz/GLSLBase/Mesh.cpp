#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

PlaneMesh::PlaneMesh()
{
}

PlaneMesh::~PlaneMesh()
{
}

bool PlaneMesh::BuildGeomData()
{
	mVertices.emplace_back(Vertex(glm::vec3(-0.5, 0, -0.5)));
	mVertices.emplace_back(Vertex(glm::vec3(0.5, 0, -0.5)));
	mVertices.emplace_back(Vertex(glm::vec3(0.5, 0, 0.5)));
	mVertices.emplace_back(Vertex(glm::vec3(-0.5, 0, 0.5)));

	mIndices.emplace_back(0);
	mIndices.emplace_back(1);
	mIndices.emplace_back(2);
	mIndices.emplace_back(0);
	mIndices.emplace_back(2);
	mIndices.emplace_back(3);

	return false;
}

bool PlaneMesh::BuildGeomData(float size, float divide_level)
{
	for (int i = 0; i < divide_level + 1; ++i)
	{
		for (int j = 0; j < divide_level + 1; ++j)
		{
			mVertices.emplace_back(Vertex(
				glm::vec3(-0.5 + (1 / (divide_level))*j, (float)(rand()%10) / 250.0f /*Randomize Height*/ , -0.5 + (1 / (divide_level))*i)*size, 
				glm::vec3(0,0,0),
				glm::vec4((1 / (divide_level))*j, (1 / (divide_level))*i, 0, 0)
				//,glm::vec4((rand()%255)/255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f,1)
				, glm::vec4(0.1,0.1,0.1, 1)
			));



		}
	}

	// for debugging
	//for (auto& e : mVerticesData)
	//{
	//	std::cout << e << " ";
	//}

	for (int i = 0; i < divide_level; ++i)
	{
		for (int j = 0; j < divide_level; ++j)
		{
	
			// 0 : (i+j) + (divide_level*i)
			// 1 : (i+j) + (divide_level*i) + 1
			// 2 : (i+1+j) + (divide_level*(i+1))
			// 3 : (i+1+j) + (divide_level*(i+1)) + 1
	
			// 0 1 2 , 1 3 2
	
			////////////<Triangle1>///////////////////////////////////////////////////////

			int index_0 = (i + 1 + j) + (divide_level * (i + 1));
			int index_1 = (i + j) + (divide_level * i) + 1;
			int index_2 = (i + j) + (divide_level * i);

			mIndices.emplace_back(index_0);
			mIndices.emplace_back(index_1);
			mIndices.emplace_back(index_2);
	
			// Calculate Normal Vector
			glm::vec3 edge1 = mVertices[index_1].GetPosition() - mVertices[index_0].GetPosition();
			glm::vec3 edge2 = mVertices[index_2].GetPosition() - mVertices[index_0].GetPosition();
			glm::vec3 new_normal = glm::normalize(glm::cross(edge1, edge2));
			
			// reCalculate Normal Vector Later
			mVertices[index_0].SetNormal(mVertices[index_0].GetNormal() + new_normal);
			mVertices[index_1].SetNormal(mVertices[index_1].GetNormal() + new_normal);
			mVertices[index_2].SetNormal(mVertices[index_2].GetNormal() + new_normal);

			////////////<Triangle2>///////////////////////////////////////////////////////

			index_0 = (i + 1 + j) + (divide_level * (i + 1));
			index_1 = (i + 1 + j) + (divide_level * (i + 1)) + 1;
			index_2 = (i + j) + (divide_level * i) + 1;

			mIndices.emplace_back(index_0);
			mIndices.emplace_back(index_1);
			mIndices.emplace_back(index_2);

			// Calculate Normal Vector
			edge1 = mVertices[index_1].GetPosition() - mVertices[index_0].GetPosition();
			edge2 = mVertices[index_2].GetPosition() - mVertices[index_0].GetPosition();
			new_normal = glm::normalize(glm::cross(edge1, edge2));

			// reCalculate Normal Vector Later
			mVertices[index_0].SetNormal(mVertices[index_0].GetNormal() + new_normal);
			mVertices[index_1].SetNormal(mVertices[index_1].GetNormal() + new_normal);
			mVertices[index_2].SetNormal(mVertices[index_2].GetNormal() + new_normal);

		}
	}

	for (auto& e : mVertices)
	{
		// Calculate Normalized Vector and Set
		e.SetNormal(glm::normalize(e.GetNormal()));
		//std::cout << e.GetNormal().x << ", " << e.GetNormal().y << ", " << e.GetNormal().z << " "  << std::endl;

		mVerticesData.emplace_back(e.GetPosition().x);
		mVerticesData.emplace_back(e.GetPosition().y);
		mVerticesData.emplace_back(e.GetPosition().z);
		mVerticesData.emplace_back(e.GetNormal().x);
		mVerticesData.emplace_back(e.GetNormal().y);
		mVerticesData.emplace_back(e.GetNormal().z);
		mVerticesData.emplace_back(e.GetTexCoord().x);
		mVerticesData.emplace_back(e.GetTexCoord().y);
		mVerticesData.emplace_back(e.GetTexCoord().z);
		mVerticesData.emplace_back(e.GetTexCoord().w);
		mVerticesData.emplace_back(e.GetVertexColor().x);
		mVerticesData.emplace_back(e.GetVertexColor().y);
		mVerticesData.emplace_back(e.GetVertexColor().z);
		mVerticesData.emplace_back(e.GetVertexColor().w);
	}
	


	//for (int i = 0; i < divide_level; ++i)
	//{
	//	if (i % 2 == 0)
	//	{
	//		for (int j = 0; j < divide_level; ++j)
	//		{
	//			// 0 : (i+j) + (divide_level*i)
	//			// 1 : (i+j) + (divide_level*i) + 1
	//			// 2 : (i+1+j) + (divide_level*(i+1))
	//			// 3 : (i+1+j) + (divide_level*(i+1)) + 1
	//
	//			// 0 2 1 3
	//			mIndices.emplace_back((i + j) + (divide_level*i));
	//			mIndices.emplace_back((i + 1 + j) + (divide_level*(i + 1)));
	//			mIndices.emplace_back((i + j) + (divide_level*i) + 1);
	//			mIndices.emplace_back((i + 1 + j) + (divide_level*(i + 1)) + 1);
	//		}
	//	}	
	//	else if (i % 2 == 1)
	//	{
	//		for (int j = divide_level - 1; j >= 0; --j)
	//		{
	//			// 0 : (i+j) + (divide_level*i)
	//			// 1 : (i+j) + (divide_level*i) + 1
	//			// 2 : (i+1+j) + (divide_level*(i+1))
	//			// 3 : (i+1+j) + (divide_level*(i+1)) + 1
	//
	//			// 1 3 0 2
	//			mIndices.emplace_back((i + j) + (divide_level*i) + 1);
	//			mIndices.emplace_back((i + 1 + j) + (divide_level*(i + 1)) + 1);
	//			mIndices.emplace_back((i + j) + (divide_level*i));
	//			mIndices.emplace_back((i + 1 + j) + (divide_level*(i + 1)));
	//		}
	//
	//	}
	//		
	//}
	//
	
	// for debugging
	//for (auto& e : mIndices)
	//{
	//	std::cout << e << " ";
	//}

	return false;
}
