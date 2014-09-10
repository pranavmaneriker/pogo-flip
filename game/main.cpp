#include<GL/glut.h>

//Custom Libraries
#include "../libs/obj_loader.cpp"
#include "../libs/window.h"
glutWindow win;

//Custom classes
#include "level.cpp"

using namespace std;



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
};

Game * g;

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

void Game::initGL()
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, win.width, win.height);
	GLfloat aspect = (GLfloat) win.width / win.height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0f, 0.1f, 0.0f, 0.5f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	//glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glEnable( GL_LIGHT0 );
	glEnable( GL_BLEND );
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glEnable( GL_COLOR_MATERIAL );
	glShadeModel( GL_SMOOTH );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_DEPTH_TEST );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glFlush(); 
}

void Game::initWindow(int *argc, char ** argv)
{
	win.width = 640;
	win.height = 480;
	win.title = "Pogo flip";
	win.field_of_view_angle = 45;
	win.z_near = 0.1f;
	win.z_far = 10.0f;
	// initialize and run program
	glutInit(argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glutDisplayFunc(display1);									// register Display Function
    	glutKeyboardFunc(keyPress1);								// register Keyboard Handler
    	glutSpecialFunc(specialKeyPress1);								// register Keyboard Handler
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
