float rotx, roty, rotz;

class Player{
	public:
	float x,y,z;
	float lookat_x,lookat_y,lookat_z;
};

class Level{
	private:
	string cur_level_path;
	string cur_level;
	Model_OBJ *room;
	Model_OBJ *inv;
	Model_OBJ *player;
	int g_rotation;
	Player *p;
	bool flipped;

	public:
	Level(string &l);	
	void display();
	void keyPress(unsigned char ch, int x, int y);
	void specialKeyPress(int key, int x, int y);
	void flip();
};
void Level::flip()
{
	flipped = !flipped;
}

Level::Level(string &l)
{
	cur_level = l;
	cur_level_path = "../rooms/" + l +"_alt.obj";
	inv = new Model_OBJ;
	player = new Model_OBJ;
	room = new Model_OBJ;
	inv->Load("../rooms/Level_1.obj");
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
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotx, 1, 0, 0);
	glTranslatef(p->x,p->y,p->z);
	player->Draw();	
	if(flipped){
		glRotatef(180,1,0,0);
	}
	//gluLookAt( p->x,p->y,p->z - 2, p->lookat_x + p->x,p->lookat_y,p->lookat_z +p->z, 0,1,0);
	room->Draw();
	glRotatef(180,1,0,0);
	inv->Draw();
	glutSwapBuffers();	
	glFlush(); 
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
		flip();
	}
	glutPostRedisplay();
}

void Level::specialKeyPress(int key,int x,int y)
{
	if ( key == GLUT_KEY_LEFT )
	{
		roty += 1.0;
		if(roty>=360)roty-=360;
	}
	if ( key == GLUT_KEY_RIGHT )
	{
		roty -= 1.0;
		if(roty<=0)roty+=360;
	}
	if ( key == GLUT_KEY_UP )
	{
		rotx += 1.0;
		if(rotx>=360)rotx-=360;
	}
	if ( key == GLUT_KEY_DOWN )
	{
		rotx -= 1.0;
		if(rotx<=0)rotx+=360;
	}
	glutPostRedisplay();
	//Handle arrow, function keys etc
}
