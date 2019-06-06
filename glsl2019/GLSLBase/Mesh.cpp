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
	//     aiImportFile(path.c_str(), aiProcess_JoinIdenticalVertices | // ������ ���ؽ� ����, �ε��� ����ȭ
    //     aiProcess_ValidateDataStructure |						    // �δ��� ����� ����
    //     aiProcess_ImproveCacheLocality |							    // ��� ������ ĳ����ġ�� ����
    //     aiProcess_RemoveRedundantMaterials |						    // �ߺ��� ���͸��� ����
    //     aiProcess_GenUVCoords |									    // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ
    //     aiProcess_TransformUVCoords |							    // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)
    //     aiProcess_FindInstances |								    // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����
    //     aiProcess_LimitBoneWeights |								    // ������ ���� ����ġ�� �ִ� 4���� ����
    //     aiProcess_OptimizeMeshes |								    // ������ ��� ���� �Ž��� ����
    //     aiProcess_GenSmoothNormals |								    // �ε巯�� �븻����(��������) ����
    //     aiProcess_SplitLargeMeshes |								    // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)
    //     aiProcess_Triangulate |									    // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)
    //     /* aiProcess_ConvertToLeftHanded |						    // D3D�� �޼���ǥ��� ��ȯ DX ����� ��� �ʿ��� */
    //     aiProcess_SortByPType);									    // ����Ÿ���� ������Ƽ��� ������ '������' �Ž��� ����
	
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
	
	// ���� �޽� ������ ��� 0 �� index �� ������ ��.

	//�ﰢ���̹Ƿ� ���� �̷�� ������ 3��
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
