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

int g_WindowSizeX = 500;
int g_WindowSizeY = 500;


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
	g_Renderer->SetFrameBuffer();
	g_Renderer->DrawSolidMesh();

	g_Renderer->SimulateParticle();
	g_Renderer->DrawParticle();

	g_Renderer->ResetFrameBuffer();
	//g_Renderer->DrawTexture(g_Renderer->GetTextureID("OldPage"), 0, 0, 200, 200);
	glutSwapBuffers();
}

void Idle(void)
{
	//std::cout << curr_theta << std::endl;
	//curr_theta = (curr_theta + target_theta)*0.00015f;
	
	float delta = 0.005f;
	curr_theta = (1 - delta)*curr_theta + delta * target_delta;
	
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	g_Renderer->GetMainCamera().MouseInput(x,y,button,state);
	if ((button == 3) || (button == 4))
	{
		if (button == 3) // UP
			//target_delta = std::min<float>(target_delta + 1, 2*M_PI/360.0f * (180.0f-1.f) ); // radian
			//target_delta = std::min<float>(target_delta + 15, 179);
			target_delta = std::min<float>(target_delta + 0.01, 1);
		else
			//target_delta = std::max<float>(target_delta - 15, 0);
			target_delta = std::max<float>(target_delta - 0.01, 0);
	}

	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	float delta = 0.1f;
	g_Renderer->GetMainCamera().KeyInput(key,x,y);
	
	float movedelta = 0.1f;
	if (key == 0x34)
	{
		AxisPivot.y = std::min<float>(AxisPivot.y + movedelta, 1);
	}
	if (key == 0x33)
	{
		AxisPivot.y = std::max<float>(AxisPivot.y - movedelta, 0);
	}
	if (key == 0x31)
	{
		AxisPivot.x = std::max<float>(AxisPivot.x - movedelta, 0);
	}
	if (key == 0x32)
	{
		AxisPivot.x = std::min<float>(AxisPivot.x + movedelta, 1);
	}
	if (key == 0x35)
	{
		AxisPivotTheta = std::max<float>(AxisPivotTheta - 10, 0);
	}
	if (key == 0x36)
	{
		AxisPivotTheta = std::min<float>(AxisPivotTheta + 10, 180);
	}

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

