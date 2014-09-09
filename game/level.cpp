class Player{
	private:
	int x,y,z;
};

class Level{
	private:
	string cur_level_path;
	string cur_level;
	Model_OBJ *room;
	Model_OBJ *player;
	int g_rotation;
		
	public:
	Level(string &l);	
	void display();
	void keyPress(unsigned char ch, int x, int y);
	void specialKeyPress(int key, int x, int y);
};

Level::Level(string &l)
{
	cur_level = l;
	cur_level_path = "../rooms/" + l +".obj";
	player = new Model_OBJ;
	room = new Model_OBJ;
	player->Load("../models/Tux.obj");
	room->Load(&cur_level_path[0]);	//&cur_level_path[0]  might avoid warning but is it safe?	
	g_rotation = 0;
}

void Level::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt( 0,1,4, 0,0,0, 0,1,0);
	glPushMatrix();
		glRotatef(g_rotation,0,1,0);
		glRotatef(90,0,1,0);
		g_rotation++;
		player->Draw();
		room->Draw();
	glPopMatrix();
	glutSwapBuffers();	
	glFlush(); 
}

void Level::keyPress(unsigned char key, int x, int y)
{
	//normal key press events
}

void Level::specialKeyPress(int key,int x,int y)
{
	//Handle arrow, function keys etc
}
