#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh()
	: mVBO(UINT32_MAX)
	, mIBO(UINT32_MAX)
{
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mIBO);
}

bool Mesh::load(const std::string& filepath)
{
	//     aiImportFile(path.c_str(), aiProcess_JoinIdenticalVertices | // 동일한 버텍스 결합, 인덱싱 최적화
    //     aiProcess_ValidateDataStructure |						    // 로더의 출력을 검증
    //     aiProcess_ImproveCacheLocality |							    // 출력 정점의 캐쉬위치를 개선
    //     aiProcess_RemoveRedundantMaterials |						    // 중복된 매터리얼 제거
    //     aiProcess_GenUVCoords |									    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
    //     aiProcess_TransformUVCoords |							    // UV 변환 처리기 (스케일링, 변환...)
    //     aiProcess_FindInstances |								    // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
    //     aiProcess_LimitBoneWeights |								    // 정점당 뼈의 가중치를 최대 4개로 제한
    //     aiProcess_OptimizeMeshes |								    // 가능한 경우 작은 매쉬를 조인
    //     aiProcess_GenSmoothNormals |								    // 부드러운 노말벡터(법선벡터) 생성
    //     aiProcess_SplitLargeMeshes |								    // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
    //     aiProcess_Triangulate |									    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
    //     /* aiProcess_ConvertToLeftHanded |						    // D3D의 왼손좌표계로 변환 DX 사용할 경우 필요함 */
    //     aiProcess_SortByPType);									    // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬
	
	const aiScene* pScene = aiImportFile(
		  filepath.c_str()
		, aiProcess_ValidateDataStructure
		| aiProcess_JoinIdenticalVertices
		| aiProcess_GenSmoothNormals
		| aiProcess_SortByPType
		| aiProcess_Triangulate
	);

	if (nullptr == pScene) return false;

	aiMesh* pMesh = (pScene->mMeshes[0]);
	
	// 다중 메시 구현할 경우 0 을 index 로 고쳐줄 것.

	//삼각형이므로 면을 이루는 꼭지점 3개
	mVertices.resize(pMesh->mNumVertices);
	for (UINT i = 0; i < pMesh->mNumVertices; ++i) {
		mVertices[i].pos = glm::vec3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
		mVertices[i].norm = glm::vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
		glm::vec2 tex;

		if (true == pMesh->HasTextureCoords(0)) {
			mVertices[i].tex_coord = glm::vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
		}
	}

	unsigned int idx = 0;
	mIndices.resize(pMesh->mNumFaces * 3);
	for (unsigned int i = 0; i < pMesh->mNumFaces; ++i) {
		const aiFace& face = pMesh->mFaces[i];
		mIndices[idx++] = face.mIndices[0];
		mIndices[idx++] = face.mIndices[1];
		mIndices[idx++] = face.mIndices[2];
	}

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

	return true;
}

void Mesh::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
}

void Mesh::render()
{
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
}
