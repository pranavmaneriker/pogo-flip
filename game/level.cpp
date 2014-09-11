#include<unistd.h>
float rotx, roty, rotz;

class Player{
	public:
	float x,y,z;
	float lookat_x,lookat_y,lookat_z;
};

float flip_angle;	

class Level{
	private:
	string cur_level_path;
	string cur_level;
	Model_OBJ *room;
	Model_OBJ *inv;
	Model_OBJ *player;
	int g_rotation;
	Player *p;

	public:
	Level(string &l);
	void display();
	void keyPress(unsigned char ch, int x, int y);
	void specialKeyPress(int key, int x, int y);
};

Level::Level(string &l)
{
	flip_angle = 0;
	cur_level = l;
	cur_level_path = "../rooms/" + l +"_alt.obj";
	inv = new Model_OBJ;
	player = new Model_OBJ;
	room = new Model_OBJ;
	inv->Load("../rooms/Level_1_test.obj");
	player->Load("../models/Tux.obj");
	room->Load(&cur_level_path[0]);	//&cur_level_path[0]  might avoid warning but is it safe?	
	g_rotation = 0;
	p = new Player;
	p->x = 0; p->y = -1; p->z = -3;	
	p->lookat_x = 0; p->lookat_y = 1; p->lookat_z = 0;
}

void Level::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(p->x,p->y,p->z);
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotx, 1, 0, 0);
	glColor3f(0,0.5,0.5);

	glPushMatrix();
	glTranslatef(0,0.02,0);
	player->Draw();	
	glPopMatrix();

	glRotatef(flip_angle,1,0,0);
	
	//gluLookAt( p->x,p->y,p->z - 2, p->lookat_x + p->x,p->lookat_y,p->lookat_z +p->z, 0,1,0);
	glColor3f(0,1,0);
	room->Draw();
	glTranslatef(0, -0.02,0);
	glRotatef(180,1,0,0);
	glColor3f(1,1,1);
	inv->Draw();
	glutSwapBuffers();	
	glColor3f(0,0,1);
	glFlush(); 
}

void rotate(int new_angle)
{
	flip_angle+=5;
	if(flip_angle >=360)flip_angle -=360; //TODO flip angle is float, might be a problem
	glutPostRedisplay();
	if(flip_angle == 180 || flip_angle == 0)
	{}
	else
	{
		glutTimerFunc(50,rotate ,1);
	}
}

void Level::keyPress(unsigned char key, int x, int y)
{
	cout<<key<<endl;
	//normal key press events
	if( key == 'w')
	{
		p->z+=0.3;
	}
	else if( key == 's')
	{
		p->z-=0.3;
	}
	else if(key == 'a')
	{
		p->x+=0.3;
	}
	else if(key == 'd')
	{
		p->x-=0.3;
	}
	else if(key == 'q')
	{
		glutTimerFunc(100, rotate , 1 );
	}
	else if(key == KEY_ESCAPE)
	{
		exit(0);
	}
	glutPostRedisplay();
}

void Level::specialKeyPress(int key,int x,int y)
{
	if ( key == GLUT_KEY_LEFT )
	{
		roty += 3.0;
		if(roty>=360)roty-=360;
	}
	if ( key == GLUT_KEY_RIGHT )
	{
		roty -= 3.0;
		if(roty<=0)roty+=360;
	}
	if ( key == GLUT_KEY_UP )
	{
		rotx += 3.0;
		if(rotx>=360)rotx-=360;
	}
	if ( key == GLUT_KEY_DOWN )
	{
		rotx -= 3.0;
		if(rotx<=0)rotx+=360;
	}
	glutPostRedisplay();
	//Handle arrow, function keys etc
}
