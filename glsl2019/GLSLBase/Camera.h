#pragma once

class Camera {
public:
	enum class ProjType{ Orthographic, Perspective };
public:
	Camera();

	void GenerateProjView(ProjType proj);
	void GenerateOrthographicProjView();
	void GeneratePerspectiveProjView();
	void RegenerateProjView();

	void Rotate(const glm::vec3& vAxis, float fAngle);

	void KeyInput(unsigned char key, int x, int y);
	void MouseInput(int x, int y, int button = -1, int state = -1);

public:
	void SetUp(const glm::vec3& vUp)		{ m_vUp = vUp; }
	void SetLook(const glm::vec3& vLook)	{ m_vLook = vLook; }
	void SetRight(const glm::vec3& vRight)	{ m_vRight = vRight; }
	void SetPos(const glm::vec3& vPos)		{ m_vPos = vPos; }
	void SetFovy(GLfloat fFovy)				{ m_fFovy = fFovy; }
	void SetAspect(GLfloat fAspect)			{ m_fAspect = fAspect; }
	void SetNear(GLfloat fNear)				{ m_fNear = fNear; }
	void SetFar(GLfloat fFar)				{ m_fFar = fFar; }

	const glm::mat4& GetProjView()	const { return m_mtxProjView; }
	const glm::vec3& GetUp()		const { return m_vUp; }
	const glm::vec3& GetLook()		const { return m_vLook; }
	const glm::vec3& GetRight()		const { return m_vRight; }
	const glm::vec3& GetPos()		const { return m_vPos; }
	GLfloat GetFovy()				const { return m_fFovy; }
	GLfloat GetAspect()				const { return m_fAspect; }
	GLfloat GetNear()				const { return m_fNear; }
	GLfloat GetFar()				const { return m_fFar; }
	
private:
	glm::vec3 m_vUp;
	glm::vec3 m_vLook;
	glm::vec3 m_vRight;
	glm::vec3 m_vPos;

	ProjType m_ProjType;

	glm::mat4 m_mtxView;
	glm::mat4 m_mtxProj;
	glm::mat4 m_mtxProjView;

	GLfloat m_fFovy;
	GLfloat m_fAspect;
	GLfloat m_fNear;
	GLfloat m_fFar;
};