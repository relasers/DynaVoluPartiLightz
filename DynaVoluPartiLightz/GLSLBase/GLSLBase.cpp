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
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"

#include "Renderer.h"

Renderer *g_Renderer = NULL;

int g_WindowSizeX = 800;
int g_WindowSizeY = 600;


float curr_theta = 0;
float target_delta = 0;

glm::vec2 AxisPivot = glm::vec2(0.5,0.5);
float AxisPivotTheta = 90;

void RenderScene(void)
{
	

	//theta += 0.00016f;
	//std::cout << theta << std::endl;
	g_Renderer->Update();
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Renderer Test
	//g_Renderer->Test();
	//g_Renderer->DrawPlaneMesh();
	//g_Renderer->SetFrameBuffer();
	g_Renderer->DrawSolidMesh();

	g_Renderer->SimulateParticle();
	g_Renderer->DrawParticle();
	g_Renderer->LightingPass();
	
	g_Renderer->ResetFrameBuffer();
	//tDelta
	
	
	//g_Renderer->DrawTexture(g_Renderer->GetTextureID("OldPage"), 0, 0, 200, 200);
	glutSwapBuffers();
}

void Idle(void)
{
	auto start = std::chrono::high_resolution_clock::now();
	RenderScene();
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
	std::string title_string = g_Renderer->GetStatString();
	title_string += std::to_string(duration.count());
	title_string += "ms";
	glutSetWindowTitle(title_string.data());
}

void MouseInput(int button, int state, int x, int y)
{
	g_Renderer->GetMainCamera().MouseInput(x,y,button,state);
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	g_Renderer->KeyInput(key,x,y);
	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

void MouseMove(int x, int y)
{
	g_Renderer->GetMainCamera().MouseInput(x, y);
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
	glutMotionFunc(MouseMove);
	glutMainLoop();

	delete g_Renderer;

	return 0;
}

