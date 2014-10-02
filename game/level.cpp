float rotx, roty, rotz;

class Player{
	public:
	float x,y,z;
	float lookat_x,lookat_y,lookat_z;
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
		tinyobj::material_t material;
	//		cout<<"No of materials :"<<materials.size()<<endl;
	//		for(int i=0;i<shapes.size(); ++i)
	//		{
	//			mesh = shapes[i].mesh;
	//			cout<<"Shape: "<<shapes[i].name<<endl;
	//			cout<<materials[mesh.material_ids[0]].name<<endl;
	//		}
	GLfloat arbit[] = {1,0.0,0.0};
		int mat_id;
		for(unsigned int i=0;i<shapes.size();++i)
		{
			mesh = shapes[i].mesh;
			mat_id = mesh.material_ids[0];
			//glColorMaterial(GL_FRONT, GL_AMBIENT);
			glColor3fv(&(arbit[0]));
			//glMaterialfv(GL_FRONT, GL_AMBIENT, &(materials[mat_id].ambient[0]));
			//cout<<(materials[mat_id].diffuse[0])<<" "<<(materials[mat_id].diffuse[1])<<" "<<(materials[mat_id].diffuse[2])<<endl;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, &(materials[mat_id].diffuse[0]));
			//glMaterialfv(GL_FRONT, GL_SHININESS, &(materials[mat_id].shininess));
			//glMaterialfv(GL_FRONT, GL_SPECULAR, &(materials[mat_id].specular[0]));
			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));	
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &(mesh.indices[0]));		
		
		}		
		glDisableClientState(GL_VERTEX_ARRAY);	
		glDisableClientState(GL_NORMAL_ARRAY);
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
	p->x = 0; p->y = -1; p->z = -3;	
	p->lookat_x = 0; p->lookat_y = 1; p->lookat_z = 0;
	random_angle = 0;
}
void Level::rotateFace()
{
	random_angle +=3;
	if(random_angle >=360) random_angle-=360;
}

void Level::display()
{
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(p->x,p->y,p->z);
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotx, 1, 0, 0);
	//glColor3f(0,0.5,0.5);

	glPushMatrix();
	glTranslatef(0,0.02,0);
	player->DrawColor();	
	glPopMatrix();

	glRotatef(flip_angle,1,0,0);
	
	//gluLookAt( p->x,p->y,p->z - 2, p->lookat_x + p->x,p->lookat_y,p->lookat_z +p->z, 0,1,0);
	//glColor3f(0,1,0);
	//room->Draw();
	glPushMatrix();
		glTranslatef(10,2,1);
		glRotatef(random_angle, 0,1,0);
		glScalef(0.5,0.5,0.5);
		//glColor3f(1,0.1,0);
	//	randomFace->Draw();
	glPopMatrix();
	glTranslatef(0, -0.02,0);
	glRotatef(180,1,0,0);
	//glColor3f(1,1,1);
	//inv->Draw();
	glutSwapBuffers();	
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
