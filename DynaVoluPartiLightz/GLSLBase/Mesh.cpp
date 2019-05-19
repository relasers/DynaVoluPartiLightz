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
	//     aiImportFile(path.c_str(), aiProcess_JoinIdenticalVertices | // 동일한 버텍스 결합, 인덱싱 최적화
    //     aiProcess_ValidateDataStructure |        // 로더의 출력을 검증
    //     aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선
    //     aiProcess_RemoveRedundantMaterials |    // 중복된 매터리얼 제거
    //     aiProcess_GenUVCoords |                    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
    //     aiProcess_TransformUVCoords |            // UV 변환 처리기 (스케일링, 변환...)
    //     aiProcess_FindInstances |                // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
    //     aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한
    //     aiProcess_OptimizeMeshes |                // 가능한 경우 작은 매쉬를 조인
    //     aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성
    //     aiProcess_SplitLargeMeshes |            // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
    //     aiProcess_Triangulate |                    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
    //     /* aiProcess_ConvertToLeftHanded |            // D3D의 왼손좌표계로 변환 DX 사용할 경우 필요함 */
    //     aiProcess_SortByPType);                    // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬
	
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
	// 다중 메시 구현할 경우 0 을 index 로 고쳐줄 것.
	//mVertices.reserve(pMesh->mNumVertices);
	//mIndices.reserve(pMesh->mNumFaces * 3);

	//삼각형이므로 면을 이루는 꼭지점 3개
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
