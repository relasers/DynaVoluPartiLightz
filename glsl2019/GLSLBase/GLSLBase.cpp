/*
Copyright 2018 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include "Renderer.h"
#include "Camera.h"

unsigned int g_ClientWidth = 500;
unsigned int g_ClientHeight = 500;

Renderer* g_Renderer = nullptr;
Camera* g_MainCamera = nullptr;

float g_RunRate = 0.0f;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Render
	//g_Renderer->DrawRect(0.5f);
	//g_Renderer->DrawPageTurning(glm::vec3(0, 1, 0), 1.0f, g_RunRate);
	//g_Renderer->DrawParticleCDTexture();
	//g_Renderer->DrawParticles();
	//g_Renderer->DrawToFrameBufferTest(0);

	g_Renderer->DrawCubeMesh();

	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	g_MainCamera->KeyInput(key, x, y);
	switch (key) {
	case VK_ESCAPE: glutExit(); break;
	}
	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	g_MainCamera->MouseInput(x, y, button, state);
	switch (button) {
	case 3: // wheel up
		g_RunRate += 0.02f;
		if (g_RunRate > 1.0f) g_RunRate = 1.0f;
		break;
	case 4: // wheel down
		g_RunRate -= 0.02f;
		if (g_RunRate < -0.75f) g_RunRate = -0.76f;
		break;
	}
	RenderScene();
}

void MouseMove(int x, int y)
{
	g_MainCamera->MouseInput(x, y);
	RenderScene();
}

int main(int argc, char **argv)
{
	// Init GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(g_ClientWidth, g_ClientHeight);
	glutCreateWindow("GLSL 2019811017");

	glewInit();
	if (GL_TRUE == glewIsSupported("GL_VERSION_4_6")) {
		std::cout << " GL Version is 4.6\n ";
	}
	else {
		std::cout << "GLEW 4.6 not supported\n ";
	}

	// Init
	g_MainCamera = new Camera();
	g_Renderer = new Renderer(g_ClientWidth, g_ClientHeight);
	g_Renderer->SetCamera(g_MainCamera);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutSpecialFunc(SpecialKeyInput);
	glutMouseFunc(MouseInput);
	glutMotionFunc(MouseMove);
	glutMainLoop();

	delete g_Renderer;
	delete g_MainCamera;

    return 0;
}

