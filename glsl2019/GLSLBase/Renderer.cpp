#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Renderer.h"

Renderer::Renderer(unsigned int client_width, unsigned int client_height)
	: m_bInit				{ false }
	, m_ClientWidth			{ 0 }
	, m_ClientHeight		{ 0 }
	, m_nTFOs				{ 0 }
	, m_pTFOs				{ nullptr }
	, m_CurrTFO				{ 0 }
	, m_pCamera				{ nullptr }
{
	Init(client_width, client_height);
}
Renderer::~Renderer()
{
	delete[] m_pTFOs;
}

void Renderer::DrawParticleCDTexture()
{
	int next_FB_idx = (m_CurrTFO + 1) % m_nTFOs;
	m_FB_ParticleCDTexture[next_FB_idx].bind();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto shader = m_ParticleCDShader.get();

	// glLinkProgram 이전에 Varying되어야함
	const GLchar* feedbackVaryings[] = { "outPos", "outDir", "outSpeed", "outCollideTime" };
	glTransformFeedbackVaryings(shader, 4, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(shader);

	glUseProgram(shader);

	if (nullptr != m_pCamera) {
		GLuint ProjViewId = glGetUniformLocation(shader, "u_ProjView");
		glUniformMatrix4fv(ProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	glUniform1i(glGetUniformLocation(shader, "u_ClientWidth"), g_ClientWidth);
	glUniform1i(glGetUniformLocation(shader, "u_ClientHeight"), g_ClientHeight);

	glUniform1i(glGetUniformLocation(shader, "u_CDTexTure"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FB_ParticleCDTexture[m_CurrTFO].get_rendertarget_buffer());

	glUniform1i(glGetUniformLocation(shader, "u_ObjectCDpos"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_FB_ObjectCDTexture.get_rendertarget_buffer(0));

	glUniform1i(glGetUniformLocation(shader, "u_ObjectCDnorm"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_FB_ObjectCDTexture.get_rendertarget_buffer(1));

	glBindBuffer(GL_ARRAY_BUFFER, m_pTFOs[m_CurrTFO].buf);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_pTFOs[next_FB_idx].tf);

	GLint aPos = glGetAttribLocation(shader, "a_Pos");
	GLint aDir = glGetAttribLocation(shader, "a_Dir");
	GLint aSpeed = glGetAttribLocation(shader, "a_Speed");
	GLint aCollideTime = glGetAttribLocation(shader, "a_CollideTime");

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
	glDisable(GL_TEXTURE1);
	glDisable(GL_TEXTURE2);

	m_FB_ParticleCDTexture[next_FB_idx].release();
	m_CurrTFO = next_FB_idx;
}

void Renderer::DrawParticles()
{
	auto shader = m_ParticleShader.get();
	glUseProgram(shader);

	if (nullptr != m_pCamera) {
		GLuint gProjViewId = glGetUniformLocation(shader, "u_ProjView");
		glUniformMatrix4fv(gProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	glUniform1i(glGetUniformLocation(shader, "u_ClientWidth"), g_ClientWidth);
	glUniform1i(glGetUniformLocation(shader, "u_ClientHeight"), g_ClientHeight);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	glUniform1i(glGetUniformLocation(shader, "u_ParticleTexTure"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FB_ParticleCDTexture[m_CurrTFO].get_rendertarget_buffer());

	glBindBuffer(GL_ARRAY_BUFFER, m_pTFOs[m_CurrTFO].buf);

	GLint aPos = glGetAttribLocation(shader, "a_Pos");
	GLint aDir = glGetAttribLocation(shader, "a_Dir");
	GLint aSpeed = glGetAttribLocation(shader, "a_Speed");
	GLint aCollideTime = glGetAttribLocation(shader, "a_CollideTime");

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

void Renderer::DrawObjectCDTexture(GameObject& obj)
{
	auto shader = m_MeshCDShader.get();
	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_ProjView"), 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_World"), 1, GL_FALSE, &obj.transform().mtx()[0][0]);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	obj.bind_resources();

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));

	obj.render();

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
}

void Renderer::DrawObject(GameObject& obj)
{
	auto shader = m_MeshShader.get();

	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_ProjView"), 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_World"), 1, GL_FALSE, &obj.transform().mtx()[0][0]);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	glUniform4f(
		glGetUniformLocation(shader, "dirLight.mLightColor")
		, m_MainDirectionalLight.mLightColor.x
		, m_MainDirectionalLight.mLightColor.y
		, m_MainDirectionalLight.mLightColor.z
		, m_MainDirectionalLight.mLightColor.w);

	glUniform3f(
		glGetUniformLocation(shader, "dirLight.mDirection")
		, m_MainDirectionalLight.mDirection.x
		, m_MainDirectionalLight.mDirection.y
		, m_MainDirectionalLight.mDirection.z);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);

	obj.bind_resources();

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));

	obj.render();

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribTexCoord);
}

void Renderer::DumpTexture(GLuint tex_id, GLuint x, GLuint y, GLuint w, GLuint h)
{
	glViewport(x, y, w, h);

	auto shader = m_TextureDumpShader.get();

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

void Renderer::DrawParticleSystem()
{
	glEnable(GL_DEPTH_TEST);

	m_FB_ObjectCDTexture.bind();
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	for(auto& obj : m_Objects)
		DrawObjectCDTexture(obj);
	m_FB_ObjectCDTexture.release();

	DrawParticleCDTexture();

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	for (auto& obj : m_Objects)
		DrawObject(obj);
	DrawParticles();

	//DumpTexture(m_FB_ParticleCDTexture[m_CurrTFO].get_rendertarget_buffer(), 0, 0, 250, 250);
	//DumpTexture(m_FB_ObjectCDTexture.get_rendertarget_buffer(1), 0, 0, 400, 400);

	glDisable(GL_DEPTH_TEST);
}

void Renderer::Init(unsigned int client_width, unsigned int client_height)
{
	// Set window size
	m_ClientWidth = client_width;
	m_ClientHeight = client_height;

	// Load shaders
	m_ParticleCDShader.compile("./Shaders/ParticleCD.vert", "./Shaders/ParticleCD.geom", "./Shaders/ParticleCD.frag");
	m_ParticleShader.compile("./Shaders/Particle.vert", "./Shaders/Particle.geom", "./Shaders/Particle.frag");
	//m_ParticleShader.compile("./Shaders/Particle.vert", "./Shaders/Particle.frag");
	m_MeshCDShader.compile("./Shaders/SolidMeshCD.vert", "./Shaders/SolidMeshCD.frag");
	m_MeshShader.compile("./Shaders/SolidMesh.vert", "./Shaders/SolidMesh.frag");
	m_TextureDumpShader.compile("./Shaders/TextureDump.vert", "./Shaders/TextureDump.frag");
	
	// Create Mesh Data
	m_CubeMesh.load("./Resource/Model/Cube.fbx");
	m_BunnyMesh.load("./Resource/Model/TinyBunny.fbx");
	m_DragonMesh.load("./Resource/Model/TinyDragonOptimized.fbx");

	// Create vertex buffer objects
	CreateRectVBO();

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

void Renderer::CreateParticleTFO()
{
	m_nTFOs = 2;
	m_pTFOs = new TFO[m_nTFOs];

	for (unsigned int i = 0; i < m_nTFOs; i++) {
		glGenTransformFeedbacks(1, &m_pTFOs[i].tf);
		glGenBuffers(1, &m_pTFOs[i].buf);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_pTFOs[i].tf);
		glBindBuffer(GL_ARRAY_BUFFER, m_pTFOs[i].buf);

		m_pTFOs[i].count = 200000;
		std::vector<Vertex> particles;
		Vertex vertex;
		for (int j = 0; j < 100000; ++j) {
			vertex.pos = glm::vec3(RAND_FLOAT(0.18f, 0.22f), RAND_FLOAT(0.0f, 20.0f), RAND_FLOAT(-0.15f, -0.05f));
			vertex.dir = glm::vec3(0, -1.0f, 0);
			vertex.speed = 0.0f;
			particles.push_back(vertex);
		}
		for (int j = 0; j < 100000; ++j) {
			vertex.pos = glm::vec3(RAND_FLOAT(-0.35f, -0.1f), RAND_FLOAT(0.0f, 20.0f), RAND_FLOAT(-0.1f, 0.1f));
			vertex.dir = glm::vec3(0, -1.0f, 0);
			vertex.speed = 0.0f;
			particles.push_back(vertex);
		}
		//for (int j = 0; j < m_pTFOs[i].count; ++j) {
		//	vertex.pos = glm::vec3(RAND_FLOAT(-0.35f, -0.1f), RAND_FLOAT(0.0f, 20.0f), RAND_FLOAT(-0.05f, 0.05f));
		//	vertex.dir = glm::vec3(0, -0, 0);
		//	vertex.speed = 0.0f;
		//	particles.push_back(vertex);
		//}
		//for (int j = 0; j < m_pTFOs[i].count; ++j) {
		//	vertex.pos = glm::vec3(RAND_FLOAT(0.18f, 0.22f), RAND_FLOAT(0.0f, 20.0f), RAND_FLOAT(-0.15f, -0.05f));
		//	vertex.dir = glm::vec3(0, -0, 0);
		//	vertex.speed = 0.0f;
		//	particles.push_back(vertex);
		//}
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_pTFOs[i].count, particles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_pTFOs[i].buf);
	}
}

void Renderer::CreateSceneObjects()
{
	m_Objects.emplace_back();
	auto& cube = m_Objects.back();
	cube.set_mesh(&m_CubeMesh);
	cube.transform().set_pos(glm::vec3(0, -0.5f, 0));
	cube.transform().set_scale(glm::vec3(0.01f, 0.001f, 0.01f));

	m_Objects.emplace_back();
	auto& bunny = m_Objects.back();
	bunny.set_mesh(&m_BunnyMesh);
	bunny.transform().set_pos(glm::vec3(-0.2f, -0.5f, 0));
	bunny.transform().set_scale(glm::vec3(0.05f, 0.05f, 0.05f));

	m_Objects.emplace_back();
	auto& dragon = m_Objects.back();
	dragon.set_mesh(&m_DragonMesh);
	dragon.transform().set_pos(glm::vec3(0.2f, -0.5f, 0));
	dragon.transform().set_scale(glm::vec3(0.05f, 0.05f, 0.05f));
}

void Renderer::CreateFrameResources()
{
	for (int i = 0; i < 2; ++i) {
		m_FB_ParticleCDTexture[i].create_rendertarget_buffer(GL_RGBA32F, m_ClientWidth, m_ClientHeight, GL_RGBA, GL_FLOAT);
		m_FB_ParticleCDTexture[i].create_depthstencil_buffer(GL_DEPTH24_STENCIL8, m_ClientWidth, m_ClientHeight, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
		m_FB_ParticleCDTexture[i].generate_framebuffer();
	}

	m_FB_ObjectCDTexture.create_rendertarget_buffer(GL_RGBA32F, m_ClientWidth, m_ClientHeight, GL_RGBA, GL_FLOAT);
	m_FB_ObjectCDTexture.create_rendertarget_buffer(GL_RGBA32F, m_ClientWidth, m_ClientHeight, GL_RGBA, GL_FLOAT);
	m_FB_ObjectCDTexture.create_depthstencil_buffer(GL_DEPTH24_STENCIL8, m_ClientWidth, m_ClientHeight, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	m_FB_ObjectCDTexture.generate_framebuffer();

	// 기본 프레임 버퍼가 나오도록 한다.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
