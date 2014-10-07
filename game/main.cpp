#include<GL/glut.h>
#include<GL/freeglut.h>
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.
#include <unistd.h>     // needed to sleep.
#include<bits/stdc++.h>

//Custom Libraries
//#include "../libs/obj_loader.cpp"
#include "../libs/tiny_obj_loader.cc"
#include "../libs/window.h"
#include<IL/il.h>
#include<IL/ilu.h>
//TODO remove SOIL
#include<SOIL/SOIL.h>

glutWindow win;

using namespace std;

#define KEY_ESCAPE 27
//Custom classes
#include "level.cpp"


class Game{
	private:
	int score;
	int health;
	void initGL();
	void initWindow(int * argc, char ** argv);
	Level *level;
	string level_name;
	
	public:
	Game();
	void start(int *argc, char ** argv);
	void keyPress(unsigned char key, int x, int y);
	void specialKeyPress(int key, int x, int y);
	void display();
	void rotateFace();
};

Game * g;

void Game::rotateFace()
{
	level->rotateFace();
}

void display1()
{
	g->display();
}

void keyPress1(unsigned char key, int x, int y)
{
	g->keyPress(key, x, y);
}

void specialKeyPress1(int key, int x, int y)
{
	g->specialKeyPress(key, x, y);
}

Game::Game()
{
	level_name = "Level_1";
	level = new Level(level_name);
}

void Game::display()
{
	level->display();
}

void Game::keyPress(unsigned char key, int x, int y)
{
	if(key == KEY_ESCAPE)
	{
		exit(0);	//add more graceful exit
	}

	level->keyPress(key,x,y);
}

void Game::specialKeyPress(int key, int x, int y)
{
	level->specialKeyPress(key,x,y);
}

GLuint texture[1];

void Game::initGL()
{
	glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

	glClearColor(0.5,0.5,0.5,1.0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45,640.0/480.0,1.0,1000.0);
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        float col[]={1.0,1.0,1.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
	glLightfv(GL_LIGHT0, GL_SPECULAR, col);
	glLightfv(GL_LIGHT0, GL_AMBIENT, col);
	//draw
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        float pos[]={-1.0,1.0,-2.0,1.0};
        glLightfv(GL_LIGHT0,GL_POSITION,pos);
}

void rotate()
{
	g->rotateFace();
	glutPostRedisplay();
}

void Game::initWindow(int *argc, char ** argv)
{
	win.width = 640;
	win.height = 480;
	win.title = "Pogo flip";
	win.field_of_view_angle = 45;
	win.z_near = 0.1f;
	win.z_far = 40.0f;
	// initialize and run program
	glutInit(argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glutDisplayFunc(&display1);									// register Display Function
    	glutKeyboardFunc(&keyPress1);								// register Keyboard Handler
    	glutSpecialFunc(&specialKeyPress1);								// register Keyboard Handler
	glutFullScreen();
	glutIdleFunc(&rotate);
	initGL();
	glutMainLoop();
}

void Game::start(int *argc, char ** argv)
{
	initWindow(argc,argv);
}

int main(int argc, char **argv)
{
	g = new Game;
	rotx = roty = rotz = 0;
	g->start(&argc, argv);
	return 0;
}
