#define PI 3.14159265359
float rotx,roty,rotz;
int co = 3;
class Player{
	public:
	float angle,ratio;
	float x,y,z;
	float lx,ly,lz;
	
	void orient(float ang)
	{
	  lx = sin(ang);
	  lz = -cos(ang);
	  glLoadIdentity();
	  gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f, 1.0f, 0.0f);
	}

	void move(int i) 
	{
	  x = x + i*(lx)*0.1;
	  z = z + i*(lz)*0.1;
	  glLoadIdentity();
	  gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f,1.0f,0.0f);
	}

	int points;
};
class Target{
	public:
	float x,y,z;
	int points;
	bool reached;
	Target (float a, float b, float c, int p)
	{
		x=a,y=b,z=c,points=p;
		reached=false;
	}
};
float flip_angle;	

class Model_OBJ{
	public:
	Model_OBJ()
	{		
	}
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	bool Load(const char* filename, const char* basepath = NULL)
	{
	  std::cout << "Loading " << filename << std::endl;
	  string err = tinyobj::LoadObj(shapes, materials, filename, basepath);
	  if (!err.empty()) {
	    std::cerr << err << std::endl;
	    return false;
	  }
	  return true;
	}
	void Draw()
	{
		
		glEnableClientState(GL_VERTEX_ARRAY);	
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tinyobj::mesh_t mesh;
		for(int i=0;i<shapes.size();++i)
		{
			mesh = shapes[i].mesh;

			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));	
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &(mesh.indices[0]));		
		//
		}		
			glDisableClientState(GL_VERTEX_ARRAY);	
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void DrawColor()
	{
		glEnableClientState(GL_VERTEX_ARRAY);	
		glEnableClientState(GL_NORMAL_ARRAY);
		tinyobj::mesh_t mesh;
		int mat;
		int oldMat = -1;
		for(int i=0;i<shapes.size();++i)
		{
			mesh = shapes[i].mesh;
			mat = mesh.material_ids[0];
			if(mat !=oldMat)
			{
				oldMat = mat;
				glMaterialfv(GL_FRONT, GL_AMBIENT, materials[mat].ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, materials[mat].diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, materials[mat].specular);
				glMaterialfv(GL_FRONT, GL_SHININESS, &materials[mat].shininess);
			}	
			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));	
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &(mesh.indices[0]));		
		//
		}		
			glDisableClientState(GL_VERTEX_ARRAY);	
			glDisableClientState(GL_NORMAL_ARRAY);
	}

	void DrawTexture()
	{
		
		glEnableClientState(GL_VERTEX_ARRAY);	
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tinyobj::mesh_t mesh;
		for(int i=0;i<shapes.size();++i)
		{
			mesh = shapes[i].mesh;

			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));	
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &(mesh.indices[0]));		
		//
		}		
			glDisableClientState(GL_VERTEX_ARRAY);	
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	}
};

class Level{
	private:
	string cur_level_path;
	string cur_level;
	Model_OBJ *room;
	Model_OBJ *inv;
	Model_OBJ *player;
	Model_OBJ *randomFace;
	
	int g_rotation;
	Player *p;
	float random_angle;

	public:
	vector<Target> targets;
	Level(string &l);
	void display();
	void keyPress(unsigned char ch, int x, int y);
	void specialKeyPress(int key, int x, int y);
	void rotateFace();
};


Level::Level(string &l)
{
	flip_angle = 0;
	cur_level = l;
	cur_level_path = "../rooms/" + l +"_alt.obj";
	inv = new Model_OBJ;
	player = new Model_OBJ;
	room = new Model_OBJ;
	randomFace = new Model_OBJ;
	inv->Load("../rooms/Level_1_test.obj");
	player->Load("../models/Tux.obj", "../models/");
	randomFace->Load("../models/monkey.obj");
	room->Load(&cur_level_path[0]);	//&cur_level_path[0]  might avoid warning but is it safe?	
	g_rotation = 0;
	p = new Player;
	p->angle=0;
	p->x = 0; p->y = 1; p->z = 0; p->points =0;
	roty=54;
//	p->curx=p->cury=0; p->curz=3.4;	
	p->lx = 1; p->ly = 0; p->lz = 0;
	random_angle = 0;
	targets.push_back(Target(5, 0.2 , 2, 20));
	targets.push_back(Target(5, 0.2 , 3, 20));
	targets.push_back(Target(5, 0.2 , 1, 20));
	
}
void Level::rotateFace()
{
	random_angle +=3;
	if(random_angle >=360) random_angle-=360;
}

void Level::display()
{
	glClearColor(0.7215,0.8627,0.9490,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
	glLoadIdentity();
	gluLookAt(p->x, p->y, p->z, p->x + p->lx,p->y + p->ly,p->z + p->lz, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
		glTranslatef(p->x,p->y+-1,p->z);
		glTranslatef(p->lx,p->ly,p->lz);
		glRotatef(-(p->angle*180/3.14),0,1,0);
		glTranslatef(0,0,-1);
		glRotatef(180,0,1,0);	
		player->DrawColor();
	glPopMatrix();
//	glRotatef(roty, 0, 1, 0);
//	glRotatef(rotx, 1, 0, 0);


//	glPushMatrix();
//		glTranslatef(0,0.02,0);
//		player->DrawColor();	
//	glPopMatrix();
	glRotatef(flip_angle,1,0,0);
	
	room->DrawColor();
//	glPushMatrix();
//		glRotatef(-roty+180, 0, 1, 0);
//		glRotatef(-rotx, 1, 0, 0);
//		glTranslatef(p->curx,p->cury,p->curz);
//		player->DrawColor();
//	glPopMatrix();
	glPushMatrix();
//		glTranslatef(10,2,1);
//		glRotatef(random_angle, 0,1,0);
//		glScalef(0.5,0.5,0.5);
//		randomFace->Draw();
	glPopMatrix();
	for(int i = 0; i < targets.size() ; i++)
	{
		if(targets[i].reached==false)
		{
			glPushMatrix();
				glTranslatef(targets[i].x,targets[i].y,targets[i].z);
				glRotatef(random_angle, 0,1,0);
				glScalef(0.25,0.25,0.25);
				randomFace->Draw();
			glPopMatrix();
		}
	}
	glTranslatef(0, -0.02,0);
	glRotatef(180,1,0,0);


	inv->Draw();
	
	
	
	//HUD (Hud dabangg dabangg)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, win.width, win.height, 0.0, -1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
	    glColor3f(0.8, 0.4, 0.4);
	    glVertex2f(0.0, 0.0);
	    glVertex2f(15*win.width/100, 0.0);
	    glVertex2f(15*win.width/100, win.height);
	    glVertex2f(0.0, win.height);
	glEnd();
	
	glBegin(GL_QUADS);
	    glColor3f(0, 0, 0.5);
	    glVertex2f(10, 400);
	    glVertex2f(15*win.width/100 - 10, 400);
	    glVertex2f(15*win.width/100 - 10, win.height-10);
	    glVertex2f(10, win.height - 10);
	glEnd();
	
	//printing text
	const char* x = "Pogo Flip\n----------------\nReach targets using \na,w,s,d keys \nto score points \n Use w to take screenshot\n Monkeys to go:";
	const char * rem="0"; //sprintf(rem,"%d", co);
	unsigned char* y;
	y = (unsigned char*) x;//strcat(x,rem);
	glColor3f(0,0,0);
	glRasterPos2i(10, 100);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, y);

	// Making sure we can render 3d again
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glutSwapBuffers();	
	glFlush(); 
	//cout<<rotx<<" "<<roty<<" "<<rotz<<" "<<p->x<<" "<<p->y<<" "<<p->z<<endl; 
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
	//normal key press events
	if( key == 'w')
	{
		p->move(1);
		//p->z+=0.3;
	}
	else if( key == 's')
	{
		p->move(-1);
		//p->z-=0.3;
	}
	else if(key == 'a')
	{
		p->angle -= 0.1f;p->orient(p->angle);
		//p->x+=0.3;
	}
	else if(key == 'd')
	{
		p->angle +=0.1f;p->orient(p->angle);
	//	p->x-=0.3;
	}
	else if(key == 'q')
	{
		glutTimerFunc(100, rotate , 1 );
	}
////////else if(key == 'i')
////////{
////////	p->curx+=0.3;
////////}
////////else if(key == 'k')
////////{
////////	p->curx-=0.3;
////////}
////////else if(key == 'j')
////////{
////////	p->curz-=0.3;
////////}
////////else if(key == 'l')
////////{
////////	p->curz+=0.3;
////////}
////////else if(key == KEY_ESCAPE)
////////{
////////	exit(0);
////////}

	co = 0;
	for(int i=0; i<targets.size(); i++)
	{
		if((targets[i].x-(p->x+p->lx))*(targets[i].x-(p->x+p->lx))+(targets[i].z-(p->lz+p->z))*(targets[i].z-(p->lz+p->z)) <= 1 && targets[i].reached == false)
		{
			targets[i].reached = true;
			p->points += targets[i].points;
		}
		else
		{
			co++;
		}
	}
	glutPostRedisplay();
}

void Level::specialKeyPress(int key,int x,int y)
{
////////if ( key == GLUT_KEY_RIGHT )
////////{
////////	roty += 3.0;
////////	if(roty>=360)roty-=360;
////////	float tx, tz;
////////	tx = p->curx;
////////	tz = p->curz;
////////	p->curx = tx*cos(3.0*PI/180) + (tz + 3.4)*sin(3.0*PI/180);
////////	p->curz = (tz + 3.4)*cos(3.0*PI/180) - tx*sin(3.0*PI/180) - 3.4;
////////}
////////if ( key == GLUT_KEY_LEFT )
////////{
////////	roty -= 3.0;
////////	if(roty<=0)roty+=360;
////////	float tx, tz;
////////	tx = p->curx;
////////	tz = p->curz;
////////	p->curx = tx*cos((360-3.0)*PI/180)+(tz + 3.4)*sin((360-3.0)*PI/180);
////////	p->curz = (tz + 3.4)*cos((360-3.0)*PI/180)-tx*sin((360-3.0)*PI/180) - 3.4;
////////}
////////if ( key == GLUT_KEY_UP )
////////{
////////	p->z+=0.3;
////////	p->curz+=0.3*cos(roty*PI/180);
////////	p->curx+=0.3*sin(roty*PI/180);
////////	//rotx += 3.0;
////////	//if(rotx>=360)rotx-=360;
////////}
////////if ( key == GLUT_KEY_DOWN )
////////{
////////	p->z-=0.3;
////////	p->curz-=0.3*cos(roty*PI/180);
////////	p->curx-=0.3*sin(roty*PI/180);
////////	//rotx -= 3.0;
////////	//if(rotx<=0)rotx+=360;
////////}
////////for(int i=0; i<targets.size(); i++)
////////{
////////	if((targets[i].x-p->curx)*(targets[i].x-p->curx)+(targets[i].z-p->curz)*(targets[i].z-p->curz) <= 1 && targets[i].reached == false)
////////	{
////////		targets[i].reached = true;
////////		p->points += targets[i].points;
////////	}
////////}
////////glutPostRedisplay();
//////////Handle arrow, function keys etc
}
