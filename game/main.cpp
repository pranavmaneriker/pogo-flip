#include<GL/glut.h>

//Custom Libraries
#include "../libs/obj_loader.cpp"
#include "../libs/window.h"

//Custom classes
#include "level.cpp"

using namespace std;


class Game{
	private:
	int score;
	int health;
	void initGL();
	void initWindow();
	Level level;
	string level_name;
	glWindow window;

	
	public:
	Game();
	void start();
	void keyPress(unsigned char key, int x, int y);
	void specialKeyPress(int key, int x, int y);

}

Game::Game()
{
	level_name = "Level_1";
	Level level = new Level(level_name);
}


Game::keyPress(unsigned char key, int x, int y)
{
	Level->keyPress(key,x,y);
}

Game::specialKeyPress(unsigned char key, int x, int y)
{
	if(key == KEY_ESCAPE)
	{
		exit(0);	//add more graceful exit
	}
	Level->specialKeyPress(key,x,y);
}

Game::initGL()
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
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glEnable( GL_LIGHT0 );
	glEnable( GL_COLOR_MATERIAL );
	glShadeModel( GL_SMOOTH );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_DEPTH_TEST );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); 
}

Game::initWindow()
{
	win.width = 640;
	win.height = 480;
	win.title = "Pogo flip";
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 10.0f;
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glutDisplayFunc(level->display);									// register Display Function
    	glutKeyboardFunc(keyPress);								// register Keyboard Handler
    	glutSpecialFunc(specialKeyPress);								// register Keyboard Handler
	initGL();
}

Game::start()
{
	initWindow();
}




int main()
{
	Game *g = new Game;
	Game->start();
	return 0;
}
