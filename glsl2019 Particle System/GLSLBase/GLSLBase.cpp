/*
Copyright 2018 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include <iostream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "Renderer.h"

timer preTimer(STEADY_CLOCK::now());
frame fps;

Renderer *g_Renderer = NULL;
int g_WindowSizeX = 500;
int g_WindowSizeY = 500;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	fps = std::chrono::duration_cast<frame>(STEADY_CLOCK::now() - preTimer);
	if (fps.count() > 0)
	{
		float timeElapsed{ std::chrono::duration_cast<ms>(fps).count() * 0.001f };
		
		preTimer = STEADY_CLOCK::now();

		// Renderer Test
		g_Renderer->DrawVertexParticleAnim(timeElapsed);

		glutSwapBuffers();
	}
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	g_Renderer->MouseInput(button, state, x, y);

	RenderScene();
}

void MouseMotion(int x, int y)
{
	g_Renderer->MouseMotion(x, y);

	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	g_Renderer->KeyInputProcessing(key);

	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(g_WindowSizeX, g_WindowSizeY);
	glutCreateWindow("GLSL KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_4_6"))
	{
		std::cout << " GL Version is 4.6\n ";
	}
	else
	{
		std::cout << "GLEW 4.6 not supported\n ";
	}

	// Initialize Renderer
	g_Renderer = new Renderer(g_WindowSizeX, g_WindowSizeY);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyInput);
	glutMotionFunc(MouseMotion);

	glutMainLoop();

	delete g_Renderer;

    return 0;
}

