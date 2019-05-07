
enum class CameraMatrixMode 
{
	Perspective,
	Orthographic
};

class Camera
{
private:
	CameraMatrixMode mMatrixMode;

	float m_WindowSizeX;
	float m_WindowSizeY;
	float mFov;

	glm::vec3 mCameraUp;
	glm::vec3 mCameraLookatPos;
	glm::vec3 mCameraPos;

	// Matrices
	glm::mat4 mViewMatrix;
	glm::mat4 mProjMatrix;
	glm::mat4 mProjViewMatrix;


public:
	Camera();
	~Camera();

public:
	void SetWindowSize(float _WindowSizeX, float _WindowSizeY);
	
	void SetCameraMode(CameraMatrixMode _CameraMode) { mMatrixMode = _CameraMode; }

	void UpdateCameraMatrix();

	void InitializeProjMatrices();
	

	glm::vec3 GetCameraUp() { return mCameraUp; }
	glm::vec3 GetCameraLookatPos() { return mCameraLookatPos; }
	glm::vec3 GetCameraPos() { return mCameraPos; }

	glm::mat4	GetViewMatrix() { return mViewMatrix; }
	glm::mat4	GetProjMatrix() { return mProjMatrix; }
	glm::mat4	GetProjViewMatrix() { return mProjViewMatrix; }

	void SetCameraPosition(glm::vec3 _newPos) { mCameraPos = _newPos; }

private:
	void InitializeOrthoProjMatrices();
	void InitializePerspectiveProjMatrices();
};
