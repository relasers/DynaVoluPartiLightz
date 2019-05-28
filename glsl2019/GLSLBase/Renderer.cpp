#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Renderer.h"
#include "LoadPng.h"


Renderer::Renderer(unsigned int client_width, unsigned int client_height)
	: m_bInit				{ false }
	, m_ClientWidth			{ 0 }
	, m_ClientHeight		{ 0 }
	, m_SolidRectShader		{ 0 }
	, m_PageTurningShader	{ 0 }
	, m_TextureDumpShader	{ 0 }
	, m_nTextures			{ 0 }
	, m_pTextures			{ nullptr }
	, m_nTFOs				{ 0 }
	, m_pTFOs				{ nullptr }
	, m_CurrTFO				{ 0 }
	, m_nFBOs				{ 0 }
	, m_pFBOs				{ nullptr }
	, m_nRenderTargets		{ 0 }
	, m_pRenderTargets		{ nullptr }
	, m_nDepthBuffers		{ 0 }
	, m_pDepthBuffers		{ nullptr }
	, m_pCamera				{ nullptr }
{
	Init(client_width, client_height);
}
Renderer::~Renderer()
{
	delete[] m_pTextures;
	delete[] m_pTFOs;
	delete[] m_pFBOs;
	delete[] m_pRenderTargets;
	delete[] m_pDepthBuffers;
}

GLuint Renderer::CreatePngTexture(const char* filePath)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned int width = 0, height = 0;
	unsigned int error = lodepng::decode(image, width, height, filePath);

	if (0 != error) {
		lodepng_error_text(error);
		assert(0 == error);
		return -1;
	}

	GLuint temp = -1;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return temp;
}

GLuint Renderer::CreateBmpTexture(const char* filePath)
{
	//Load Bmp: Load file and decode image.
	unsigned int width = 0, height = 0;
	unsigned char* bmp = loadBMPRaw(filePath, width, height);

	if (nullptr == bmp) {
		std::cout << "Error while loading bmp file : " << filePath << std::endl;
		assert(nullptr != bmp);
		return -1;
	}

	GLuint temp = -1;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp);

	delete[] bmp;

	return temp;
}

void Renderer::DrawRect(float size)
{
	glUseProgram(m_SolidRectShader);
	
	if (nullptr != m_pCamera) {
		GLuint gProjViewId = glGetUniformLocation(m_SolidRectShader, "u_ProjView");
		glUniformMatrix4fv(gProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}
	
	GLuint attribSize = glGetUniformLocation(m_SolidRectShader, "u_Size");
	glUniform1f(attribSize, size);

	GLuint attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboRect.buf);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, VEC3F_SIZE, 0);

	glDrawArrays(GL_TRIANGLES, 0, m_vboRect.count);

	glDisableVertexAttribArray(attribPosition);
}

void Renderer::DrawPageTurning(const glm::vec3& axis, float radius, float run_rate)
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_PageTurningShader);

	glUniform1i(glGetUniformLocation(m_PageTurningShader, "u_TexTure0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pTextures[0]);

	glUniform1i(glGetUniformLocation(m_PageTurningShader, "u_TexTure1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pTextures[1]);
	
	GLuint AxisId = glGetUniformLocation(m_PageTurningShader, "u_Axis");
	glUniform3fv(AxisId, 1, &axis[0]);
	GLuint RadiusId = glGetUniformLocation(m_PageTurningShader, "u_Radius");
	glUniform1f(RadiusId, radius);
	GLuint RunRateId = glGetUniformLocation(m_PageTurningShader, "u_RunRate");
	glUniform1f(RunRateId, run_rate);

	if (nullptr != m_pCamera) {
		GLuint ProjViewId = glGetUniformLocation(m_PageTurningShader, "u_ProjView");
		glUniformMatrix4fv(ProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	GLuint attribPosition = glGetAttribLocation(m_PageTurningShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboProxyGeo.buf);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, VEC3F_SIZE, 0);

	//glDrawArrays(GL_LINE_STRIP, 0, m_vboProxyGeo.count);
	glDrawArrays(GL_TRIANGLES, 0, m_vboProxyGeo.count);

	glDisableVertexAttribArray(attribPosition);
}

void Renderer::DrawParticleCDTexture()
{
	// glLinkProgram 이전에 Varying되어야함
	const GLchar* feedbackVaryings[] = { "outPos", "outDir", "outSpeed", "outCollideTime" };
	glTransformFeedbackVaryings(m_ParticleCDShader, 4, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_ParticleCDShader);

	glUseProgram(m_ParticleCDShader);

	if (nullptr != m_pCamera) {
		GLuint ProjViewId = glGetUniformLocation(m_ParticleCDShader, "u_ProjView");
		glUniformMatrix4fv(ProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}
	GLuint ClientWidthId = glGetUniformLocation(m_ParticleCDShader, "u_ClientWidth");
	glUniform1i(ClientWidthId, g_ClientWidth);
	GLuint ClientHeightId = glGetUniformLocation(m_ParticleCDShader, "u_ClientHeight");
	glUniform1i(ClientHeightId, g_ClientHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pRenderTargets[m_CurrTFO]);
	glUniform1i(glGetUniformLocation(m_ParticleCDShader, "u_CDTexTure"), 0);

	GLint aPos = glGetAttribLocation(m_ParticleCDShader, "a_Pos");
	GLint aDir = glGetAttribLocation(m_ParticleCDShader, "a_Dir");
	GLint aSpeed = glGetAttribLocation(m_ParticleCDShader, "a_Speed");
	GLint aCollideTime = glGetAttribLocation(m_ParticleCDShader, "a_CollideTime");

	glBindBuffer(GL_ARRAY_BUFFER, m_pTFOs[m_CurrTFO].buf);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_pTFOs[(m_CurrTFO + 1) % m_nTFOs].tf);

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aDir);
	glEnableVertexAttribArray(aSpeed);
	glEnableVertexAttribArray(aCollideTime);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(aDir, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(aSpeed, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(aCollideTime, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 7));

	// Rasterazer
	glBeginTransformFeedback(GL_POINTS);

	glPointSize(1.f);
	static bool sFirst = true;
	if (true == sFirst) {
		glDrawArrays(GL_POINTS, 0, m_pTFOs[m_CurrTFO].count);
		sFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, m_pTFOs[m_CurrTFO].tf);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aDir);
	glDisableVertexAttribArray(aSpeed);
	glDisableVertexAttribArray(aCollideTime);
	glDisable(GL_TEXTURE0);

}
void Renderer::DrawParticles()
{
	glUseProgram(m_ParticleShader);

	if (nullptr != m_pCamera) {
		GLuint gProjViewId = glGetUniformLocation(m_ParticleShader, "u_ProjView");
		glUniformMatrix4fv(gProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	GLuint ClientWidthId = glGetUniformLocation(m_ParticleShader, "u_ClientWidth");
	glUniform1i(ClientWidthId, g_ClientWidth);
	GLuint ClientHeightId = glGetUniformLocation(m_ParticleShader, "u_ClientHeight");
	glUniform1i(ClientHeightId, g_ClientHeight);

	GLint aPos = glGetAttribLocation(m_ParticleShader, "a_Pos");
	GLint aDir = glGetAttribLocation(m_ParticleShader, "a_Dir");
	GLint aSpeed = glGetAttribLocation(m_ParticleShader, "a_Speed");
	GLint aCollideTime = glGetAttribLocation(m_ParticleShader, "a_CollideTime");

	glUniform1i(glGetUniformLocation(m_ParticleShader, "u_CDTexTure"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pRenderTargets[m_CurrTFO]);

	glBindBuffer(GL_ARRAY_BUFFER, m_pTFOs[m_CurrTFO].buf);

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aDir);
	glEnableVertexAttribArray(aSpeed);
	glEnableVertexAttribArray(aCollideTime);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(aDir, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(aSpeed, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(aCollideTime, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 7));

	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 0, m_pTFOs[m_CurrTFO].count);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aDir);
	glDisableVertexAttribArray(aSpeed);
	glDisableVertexAttribArray(aCollideTime);
	glDisable(GL_TEXTURE0);
}
void Renderer::DrawToFrameBufferTest(GLuint frame_buffer_id)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_pFBOs[(m_CurrTFO + 1) % m_nTFOs]);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);

	DrawCubeMeshCDTexture();
	DrawParticleCDTexture();

	m_CurrTFO = (m_CurrTFO + 1) % m_nTFOs;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	DrawCubeMesh();
	DrawParticles();

	DumpTexture(m_pRenderTargets[m_CurrTFO], 0, 0, 200, 200);
}

void Renderer::DumpTexture(GLuint tex_id, GLuint x, GLuint y, GLuint w, GLuint h)
{
	glViewport(x, y, w, h);

	GLuint shader{ m_TextureDumpShader };

	glUseProgram(shader);

	int uniformTexture = glGetUniformLocation(shader, "u_Texture");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glUniform1i(uniformTexture, 0);

	int posId = glGetAttribLocation(shader, "a_Position");

	glEnableVertexAttribArray(posId);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboRect.buf);
	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, VEC3F_SIZE, 0);

	glDrawArrays(GL_TRIANGLES, 0, m_vboRect.count);

	glDisableVertexAttribArray(posId);

	glViewport(0, 0, m_ClientWidth, m_ClientHeight);
}

void Renderer::DrawCubeMesh()
{
	GLuint shader = m_MeshShader;
	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");
	int attribVertColor = glGetAttribLocation(shader, "a_VertColor");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint world = glGetUniformLocation(shader, "u_World");

	GLuint cameraPos = glGetUniformLocation(shader, "u_CameraPos");
	GLuint mainDirLight_Color = glGetUniformLocation(shader, "dirLight.mLightColor");
	GLuint mainDirLight_Dir = glGetUniformLocation(shader, "dirLight.mDirection");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	glUniformMatrix4fv(world, 1, GL_FALSE, &m_GameObjects[OBJECT_TYPE::CUBE].GetTransform()->GetModelMatrix()[0][0]);

	glUniform3f(cameraPos, 
		m_pCamera->GetPos().x,
		m_pCamera->GetPos().y,
		m_pCamera->GetPos().z);

	glUniform4f(mainDirLight_Color, 
		m_MainDirectionalLight.mLightColor.x,
		m_MainDirectionalLight.mLightColor.y,
		m_MainDirectionalLight.mLightColor.z,
		m_MainDirectionalLight.mLightColor.w);

	glUniform3f(mainDirLight_Dir, 
		m_MainDirectionalLight.mDirection.x,
		m_MainDirectionalLight.mDirection.y,
		m_MainDirectionalLight.mDirection.z);

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);
	glEnableVertexAttribArray(attribVertColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[OBJECT_TYPE::CUBE]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribVertColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (GLvoid*)(sizeof(float) * 10));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO[OBJECT_TYPE::CUBE]);
	glLineWidth(2);

	glDrawElements(GL_TRIANGLES, m_Meshes[OBJECT_TYPE::CUBE]->GetIndicesVector().size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribTexCoord);
	glDisableVertexAttribArray(attribVertColor);

}
void Renderer::DrawCubeMeshCDTexture()
{
	GLuint shader = m_MeshCDShader;
	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");

	GLuint projView = glGetUniformLocation(shader, "u_ProjView");
	GLuint world = glGetUniformLocation(shader, "u_World");

	GLuint cameraPos = glGetUniformLocation(shader, "u_CameraPos");

	glUniformMatrix4fv(projView, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	glUniformMatrix4fv(world, 1, GL_FALSE, &m_GameObjects[OBJECT_TYPE::CUBE].GetTransform()->GetModelMatrix()[0][0]);

	glUniform3f(cameraPos,
		m_pCamera->GetPos().x,
		m_pCamera->GetPos().y,
		m_pCamera->GetPos().z);


	glEnableVertexAttribArray(attribPosition);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[OBJECT_TYPE::CUBE]);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO[OBJECT_TYPE::CUBE]);

	glDrawElements(GL_TRIANGLES, m_Meshes[OBJECT_TYPE::CUBE]->GetIndicesVector().size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(attribPosition);

}

void Renderer::Init(unsigned int client_width, unsigned int client_height)
{
	// Set window size
	m_ClientWidth = client_width;
	m_ClientHeight = client_height;

	// Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vert", "./Shaders/SolidRect.frag");
	m_PageTurningShader = CompileShaders("./Shaders/ProxyGeometry.vert", "./Shaders/ProxyGeometry.frag");
	m_TextureDumpShader = CompileShaders("./Shaders/TextureDump.vert", "./Shaders/TextureDump.frag");
	m_ParticleCDShader = CompileShaders("./Shaders/ParticleCD.vert", "./Shaders/ParticleCD.geom", "./Shaders/ParticleCD.frag");
	//m_ParticleShader = CompileShaders("./Shaders/Particle.vert", "./Shaders/Particle.geom", "./Shaders/Particle.frag");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vert", "./Shaders/Particle.frag");
	m_MeshShader = CompileShaders("./Shaders/SolidMesh.vert", "./Shaders/SolidMesh.frag");
	m_MeshCDShader = CompileShaders("./Shaders/SolidMeshCD.vert", "./Shaders/SolidMeshCD.frag");
	
	// Load textures
	m_nTextures = 2;
	m_pTextures = new GLuint[m_nTextures];
	m_pTextures[0] = CreatePngTexture("aa.png");
	m_pTextures[1] = CreatePngTexture("aa2.png");

	// Create Mesh Data
	CreateGeometryMeshDatas();

	// Create vertex buffer objects
	CreateRectVBO();
	CreateProxyGeometryVBO();
	CreateVBOandIBObyLoadMeshs();

	// Create transform feedback objects
	CreateParticleTFO();

	// Create frame buffer objects
	CreateFrameResources();

	m_MainDirectionalLight.mLightColor = glm::vec4(255.f, 244.f, 214.f, 255.f);
	m_MainDirectionalLight.mLightColor /= 255.f;
	m_MainDirectionalLight.mDirection = glm::normalize(glm::vec3(25, -30, 0));

	// GameObject
	CreateSceneObjects();
}

bool Renderer::ReadFile(std::string& out_data, const char* filename)
{
	std::ifstream file(filename);

	if (true == file.fail()) {
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}

	std::string line;
	while (getline(file, line)) {
		out_data.append(line.c_str());
		out_data.append("\n");
	}
	return true;
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (GL_FALSE == ShaderObj) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;

	GLint Lengths[1];
	Lengths[0] = static_cast<GLint>(strlen(pShaderText));

	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	GLint success = GL_FALSE;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (GL_FALSE == success) {
		GLchar InfoLog[LOG_SIZE];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, LOG_SIZE, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint Renderer::CompileShaders(const char* filenameVS, const char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (GL_FALSE == ShaderProgram) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (false == ReadFile(vs, filenameVS)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (false == ReadFile(fs, filenameFS)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = GL_FALSE;
	GLchar ErrorLog[LOG_SIZE] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);

	if (GL_FALSE == success) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (GL_FALSE == success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

GLuint Renderer::CompileShaders(const char * filenameVS, const char * filenameGS, const char * filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (GL_FALSE == ShaderProgram) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, gs, fs;

	if (false == ReadFile(vs, filenameVS)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	if (false == ReadFile(gs, filenameGS)) {
		printf("Error compiling geometry shader\n");
		return -1;
	};

	if (false == ReadFile(fs, filenameFS)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 기하 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, gs.c_str(), GL_GEOMETRY_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = GL_FALSE;
	GLchar ErrorLog[LOG_SIZE] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);

	if (GL_FALSE == success) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (GL_FALSE == success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

unsigned char* Renderer::loadBMPRaw(const char * img_path, unsigned int& outWidth, unsigned int& outHeight)
{
	std::cout << "Loading bmp file " << img_path << " ... " << std::endl;
	outWidth = -1;
	outHeight = -1;

	// Data read from the header of the BMP file
	const unsigned int header_size = 54;
	unsigned char header[header_size];
	unsigned int dataPos = 0;
	unsigned int imageSize = 0;

	// Actual RGB data
	unsigned char* data = nullptr;

	// Open the file
	FILE* file = nullptr;
	fopen_s(&file, img_path, "rb");
	if (nullptr == file) {
		std::cout << "Image could not be opened, " << img_path << " is missing. " << std::endl;
		return NULL;
	}

	if (54 != fread(header, 1, header_size, file)) {
		std::cout << img_path << " is not a correct BMP file. " << std::endl;
		return nullptr;
	}

	if ('B' != header[0] || 'M' != header[1]) {
		std::cout << img_path << " is not a correct BMP file. " << std::endl;
		return nullptr;
	}

	if (0 != (*reinterpret_cast<int*>(&(header[0x1E])))) {
		std::cout << img_path << " is not a correct BMP file. " << std::endl;
		return nullptr;
	}

	if (24 != (*reinterpret_cast<int*>(&(header[0x1C])))) {
		std::cout << img_path << " is not a correct BMP file. " << std::endl;
		return nullptr;
	}

	dataPos = *reinterpret_cast<int*>(&(header[0x0A]));
	imageSize = *reinterpret_cast<int*>(&(header[0x22]));
	outWidth = *reinterpret_cast<int*>(&(header[0x12]));
	outHeight = *reinterpret_cast<int*>(&(header[0x16]));

	if (0 == imageSize) imageSize = outWidth * outHeight * 3;
	if (0 == dataPos) dataPos = header_size;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);
	fclose(file);

	std::cout << img_path << " is succesfully loaded. " << std::endl;

	return data;
}

void Renderer::CreateGeometryMeshDatas()
{
	m_Meshes[OBJECT_TYPE::CUBE] = std::make_unique<Mesh>("Cube", std::string("./Resource/Model/mainGeom.fbx"));
}

void Renderer::CreateRectVBO()
{
	m_vboRect.count = 6;
	float rect[] = {
		// Triangle1
		  -1.0f, -1.0f, 0.f
		, -1.0f, +1.0f, 0.f
		, +1.0f, +1.0f, 0.f

		// Triangle2
		, +1.0f, -1.0f, 0.f
		, -1.0f, -1.0f, 0.f
		, +1.0f, +1.0f, 0.f,
	};

	glGenBuffers(1, &m_vboRect.buf);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboRect.buf);
	glBufferData(GL_ARRAY_BUFFER, VEC3F_SIZE * m_vboRect.count, rect, GL_STATIC_DRAW);
}

void Renderer::CreateProxyGeometryVBO()
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	int pointCountX = 32;
	int pointCountY = 32;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	m_vboProxyGeo.count = (pointCountX - 1)*(pointCountY - 1) * 2 * 3;
	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[m_vboProxyGeo.count * 3];

	//Prepare points
	for (int x = 0; x < pointCountX; ++x) {
		for (int y = 0; y < pointCountY; ++y) {
			point[(pointCountX * y + x) * 2 + 0] = basePosX + width * (x / static_cast<float>(pointCountX - 1));
			point[(pointCountX * y + x) * 2 + 1] = basePosY + height * (y / static_cast<float>(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; ++x) {
		for (int y = 0; y < pointCountY - 1; ++y) {
			//Triangle part 1
			vertices[vertIndex++] = point[(pointCountX * y + x) * 2 + 0];
			vertices[vertIndex++] = point[(pointCountX * y + x) * 2 + 1];
			vertices[vertIndex++] = 0.f;			   	 
			vertices[vertIndex++] = point[(pointCountX * (y + 1) + (x + 1)) * 2 + 0];
			vertices[vertIndex++] = point[(pointCountX * (y + 1) + (x + 1)) * 2 + 1];
			vertices[vertIndex++] = 0.f;			   	 
			vertices[vertIndex++] = point[(pointCountX * (y + 1) + x) * 2 + 0];
			vertices[vertIndex++] = point[(pointCountX * (y + 1) + x) * 2 + 1];
			vertices[vertIndex++] = 0.f;

			//Triangle part 2
			vertices[vertIndex++] = point[(pointCountX * y + x) * 2 + 0];
			vertices[vertIndex++] = point[(pointCountX * y + x) * 2 + 1];
			vertices[vertIndex++] = 0.f;			   	 
			vertices[vertIndex++] = point[(pointCountX * y + (x + 1)) * 2 + 0];
			vertices[vertIndex++] = point[(pointCountX * y + (x + 1)) * 2 + 1];
			vertices[vertIndex++] = 0.f;			   	 
			vertices[vertIndex++] = point[(pointCountX * (y + 1) + (x + 1)) * 2 + 0];
			vertices[vertIndex++] = point[(pointCountX * (y + 1) + (x + 1)) * 2 + 1];
			vertices[vertIndex++] = 0.f;
		}
	}

	glGenBuffers(1, &m_vboProxyGeo.buf);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboProxyGeo.buf);
	glBufferData(GL_ARRAY_BUFFER, VEC3F_SIZE * m_vboProxyGeo.count, vertices, GL_STATIC_DRAW);

	delete[] point;
	delete[] vertices;
}

void Renderer::CreateParticleTFO()
{
	m_nTFOs = 2;
	m_pTFOs = new TFO[m_nTFOs];

	for (unsigned int i = 0; i < m_nTFOs; i++) {
		glGenTransformFeedbacks(1, &m_pTFOs[i].tf);
		glGenBuffers(1, &m_pTFOs[i].buf);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_pTFOs[i].tf);
		glBindBuffer(GL_ARRAY_BUFFER, m_pTFOs[i].buf);

		m_pTFOs[i].count = 20000;
		std::vector<Vertex> particles;
		//for (int j = 0; j < m_pTFOs[i].count; ++j) {
		//	particles.emplace_back();
		//}
		Vertex vertex;
		for (int i = 0; i < 10000; ++i) {
			vertex.pos = glm::vec3(RAND_FLOAT(0.15f, 0.25f), RAND_FLOAT(-0.2f, 0.2f), RAND_FLOAT(-0.2f, 0.2f));
			vertex.dir = glm::vec3(-1.0f, 0, 0);
			vertex.speed = 0.02f;
			particles.push_back(vertex);
		}
		for (int i = 0; i < 10000; ++i) {
			vertex.pos = glm::vec3(-RAND_FLOAT(0.15f, 0.25f), RAND_FLOAT(-0.2f, 0.2f), RAND_FLOAT(-0.2f, 0.2f));
			vertex.dir = glm::vec3(1.0f, 0, 0);
			vertex.speed = 0.02f;
			particles.push_back(vertex);
		}
		//vertex.pos = glm::vec3(0.0f, 0, 0);
		//vertex.dir = glm::vec3(0.0f, 0, 0);
		//vertex.speed = 0.0f;
		//particles.push_back(vertex);
		//vertex.pos = glm::vec3(-0.1f, 0, 0);
		//vertex.dir = glm::vec3(1.0f, 0, 0);
		//vertex.speed = 0.01f;
		//particles.push_back(vertex);

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_pTFOs[i].count, particles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_pTFOs[i].buf);
	}
}

void Renderer::CreateVBOandIBObyLoadMeshs()
{
	for (auto& p : m_Meshes) {
		GLuint VBO;
		GLuint IBO;

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, p.second->GetVerticesData().size() * sizeof(float), &p.second->GetVerticesData().at(0), GL_STATIC_DRAW);

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, p.second->GetIndicesVector().size() * sizeof(unsigned int), &p.second->GetIndicesVector().at(0), GL_STATIC_DRAW);


		m_VBO[p.first] = VBO;
		m_IBO[p.first] = IBO;
	}
}

void Renderer::CreateSceneObjects()
{
	GameObject Cube;
	m_GameObjects[OBJECT_TYPE::CUBE] = Cube;
	m_GameObjects[OBJECT_TYPE::CUBE].SetMesh(m_Meshes[OBJECT_TYPE::CUBE].get());
	m_GameObjects[OBJECT_TYPE::CUBE].SetPosition(glm::vec3(0, -0.5f, -0.5f));
	m_GameObjects[OBJECT_TYPE::CUBE].SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
}


void Renderer::CreateFrameResources()
{
	// RenderTargets
	m_nRenderTargets = 2;
	m_pRenderTargets = new GLuint[m_nRenderTargets];
	
	glGenTextures(1, &m_pRenderTargets[0]);
	glBindTexture(GL_TEXTURE_2D, m_pRenderTargets[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_ClientWidth, m_ClientHeight, 0, GL_RGBA, GL_FLOAT, 0);

	glGenTextures(1, &m_pRenderTargets[1]);
	glBindTexture(GL_TEXTURE_2D, m_pRenderTargets[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_ClientWidth, m_ClientHeight, 0, GL_RGBA, GL_FLOAT, 0);

	// Depth Buffer
	m_nDepthBuffers = 2;
	m_pDepthBuffers = new GLuint[m_nDepthBuffers];
	for (int i = 0; i < m_nDepthBuffers; ++i) {
		glGenTextures(1, &m_pDepthBuffers[i]);
		glBindTexture(GL_TEXTURE_2D, m_pDepthBuffers[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_ClientWidth, m_ClientHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}

	// FBO
	m_nFBOs = 2;
	m_pFBOs = new GLuint[m_nFBOs];
	glGenFramebuffers(1, &m_pFBOs[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pFBOs[0]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_pRenderTargets[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_pDepthBuffers[0], 0);

	glGenFramebuffers(1, &m_pFBOs[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pFBOs[1]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_pRenderTargets[1], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_pDepthBuffers[1], 0);


	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (error != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Bind framebuffer object error \n";
	}

	// 기본 프레임 버퍼가 나오도록 한다.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
