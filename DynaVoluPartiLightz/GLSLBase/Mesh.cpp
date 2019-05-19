#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(std::string _name, std::string path)
{
	mName = _name;
	loadOBJ(path);
}

Mesh::~Mesh()
{
}

bool Mesh::loadOBJ(std::string path)
{
	//     aiImportFile(path.c_str(), aiProcess_JoinIdenticalVertices | // ������ ���ؽ� ����, �ε��� ����ȭ
    //     aiProcess_ValidateDataStructure |        // �δ��� ����� ����
    //     aiProcess_ImproveCacheLocality |        // ��� ������ ĳ����ġ�� ����
    //     aiProcess_RemoveRedundantMaterials |    // �ߺ��� ���͸��� ����
    //     aiProcess_GenUVCoords |                    // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ
    //     aiProcess_TransformUVCoords |            // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)
    //     aiProcess_FindInstances |                // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����
    //     aiProcess_LimitBoneWeights |            // ������ ���� ����ġ�� �ִ� 4���� ����
    //     aiProcess_OptimizeMeshes |                // ������ ��� ���� �Ž��� ����
    //     aiProcess_GenSmoothNormals |            // �ε巯�� �븻����(��������) ����
    //     aiProcess_SplitLargeMeshes |            // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)
    //     aiProcess_Triangulate |                    // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)
    //     /* aiProcess_ConvertToLeftHanded |            // D3D�� �޼���ǥ��� ��ȯ DX ����� ��� �ʿ��� */
    //     aiProcess_SortByPType);                    // ����Ÿ���� ������Ƽ��� ������ '������' �Ž��� ����
	
	const aiScene* pScene = aiImportFile(path.c_str(), aiProcess_ValidateDataStructure | aiProcess_JoinIdenticalVertices
		| aiProcess_GenSmoothNormals | aiProcess_SortByPType | aiProcess_Triangulate);


	if(pScene) {
		aiMesh* pMesh = (pScene->mMeshes[0]);
		InitLoadMesh(pMesh);
		mNumVertices = mVertices.size();
	}

	//if (pScene) delete pScene;

	return true;
}

void Mesh::InitLoadMesh(const aiMesh* pMesh)
{
	// ���� �޽� ������ ��� 0 �� index �� ������ ��.
	//mVertices.reserve(pMesh->mNumVertices);
	//mIndices.reserve(pMesh->mNumFaces * 3);

	//�ﰢ���̹Ƿ� ���� �̷�� ������ 3��
	for (UINT i = 0; i < pMesh->mNumVertices; ++i) {

		glm::vec3 pos(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
		glm::vec3 normal(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
		glm::vec2 tex;

		if (pMesh->HasTextureCoords(0))
		{
			tex = glm::vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
		}
		else
		{
			tex = glm::vec2(0.0f, 0.0f);
		}
		Vertex newVertex;
		newVertex.SetPosition(pos);
		newVertex.SetNormal(normal);
		newVertex.SetTexCoord0(glm::vec4(tex,0,0));

		mVertices.emplace_back(newVertex);
	}


	for (UINT i = 0; i < pMesh->mNumFaces; ++i) {
		const aiFace& face = pMesh->mFaces[i];
		mIndices.emplace_back(face.mIndices[0]);
		mIndices.emplace_back(face.mIndices[1]);
		mIndices.emplace_back(face.mIndices[2]);
	}

	UpdateVerticesDataForVBO();
}

void Mesh::UpdateVerticesDataForVBO()
{



	//// for debugging
	//for (auto& e : mIndices)
	//{
	//	std::cout << e << ", ";
	//}

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

	UpdateVerticesDataForVBO();

	
	


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
	
	

	return false;
}
