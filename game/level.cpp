#define PI 3.14159265359
float rotx, roty, rotz;

class Player{
	public:
	float x,y,z;
	int points;
	float curx, cury, curz;
	float lookat_x,lookat_y,lookat_z;
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

#include<SOIL/SOIL.h>

void saveScreenShot()
{
int	save_result = SOIL_save_screenshot
	(
		"awesomenessity.bmp",
		SOIL_SAVE_TYPE_BMP,
		0, 0, 1024, 768
	);
}

class Model_OBJ{
	public:
	Model_OBJ()
	{		
	}
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	map<string, GLuint> tex_2d_diffuse;


	bool Load(const char* filename, const char* basepath = NULL)
	{
	  std::cout << "Loading " << filename << std::endl;
	  string err = tinyobj::LoadObj(shapes, materials, filename, basepath);
		/* load an image file directly as a new OpenGL texture */
	  
	  if (!err.empty()) {
	    std::cerr << err << std::endl;
	    return false;
	  }

	
	  return true;
	}
	
	bool LoadTexture(const char * basepath)	//assume one material per object
	{	
	  tinyobj::material_t temp;

          string texpath;
          for(unsigned int i=0;i<shapes.size(); ++i)
          {
        	  if(!shapes[i].mesh.material_ids.empty() && !shapes[i].mesh.texcoords.empty())
        	  {
        		 temp = materials[shapes[i].mesh.material_ids[0]];
        		 cout<<"Loading for: "<<shapes[i].name<<endl;
        		 texpath = temp.diffuse_texname;
        		 cout<<"Path: "<<texpath<<endl;
     	         if(!loadtimg(&(texpath[0]) , temp.name))
     	         {
     	        	 cout<<"Could not load "<<temp.name<<" texture :"<<temp.diffuse_texname<<endl;
     	         }
        	  }
          }
	}
	
	bool loadtimg(const char * texname, string& matname)
	{
		cout<<"Trying to load: "<<texname<<endl;
        	GLuint tex_2d_diffuse_temp = SOIL_load_OGL_texture
        	(
			"test.jpg",
        	//	texname ,
        		SOIL_LOAD_AUTO,
        		SOIL_CREATE_NEW_ID,
        		SOIL_FLAG_INVERT_Y
        	);
        	if(tex_2d_diffuse_temp == 0){
			cerr << "SOIL loading error: '" << SOIL_last_result()<<endl;
        		return false;
        	}
        	tex_2d_diffuse[matname] = tex_2d_diffuse_temp;
		    glBindTexture(GL_TEXTURE_2D, tex_2d_diffuse_temp);
		    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
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
				if(!mesh.texcoords.empty())
				{
					glBindTexture(GL_TEXTURE_2D, tex_2d_diffuse[materials[mat].name]);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
					glTexCoordPointer(2, GL_FLOAT, 0, &(mesh.texcoords[0])); 
				}
			}	


			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			if(!mesh.normals.empty())
			{
				glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));	
			}
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &(mesh.indices[0]));		

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
			glTexCoordPointer(2,GL_FLOAT, 0, &(mesh.texcoords[0]));
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

	


//	inv->Load("../rooms/Level_1_test.obj");
	//inv->Load("../rooms/SnowTerrain/SnowTerrain.obj", "../rooms/SnowTerrain/");

	

	player->Load("../models/Tux.obj", "../models/");
	randomFace->Load("../models/monkey.obj");
	//room->Load(&cur_level_path[0]);	//&cur_level_path[0]  might avoid warning but is it safe?	
	g_rotation = 0;
	p = new Player;
	p->x = 0; p->y = -1; p->z = -3.4; p->points =0;
	roty=54;
	p->curx=p->cury=0; p->curz=3.4;	
	p->lookat_x = 0; p->lookat_y = 1; p->lookat_z = 0;
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
	glClearColor(0,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
	glLoadIdentity();
	glTranslatef(p->x,p->y,p->z);
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotx, 1, 0, 0);


	glPushMatrix();
		glTranslatef(0,0.02,0);
		//player->DrawColor();	
	glPopMatrix();
	glRotatef(flip_angle,1,0,0);
	
	room->DrawColor();
	glPushMatrix();
		glRotatef(-roty+180, 0, 1, 0);
		glRotatef(-rotx, 1, 0, 0);
		glTranslatef(p->curx,p->cury,p->curz);
		player->DrawColor();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(10,2,1);
		glRotatef(random_angle, 0,1,0);
		glScalef(0.5,0.5,0.5);
		randomFace->Draw();
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
	//glRotatef(180,1,0,0);
	//glColor3f(1,1,1);
	//inv->DrawColor();
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
	else if(key == 'i')
	{
		p->curx+=0.3;
	}
	else if(key == 'k')
	{
		p->curx-=0.3;
	}
	else if(key == 'j')
	{
		p->curz-=0.3;
	}
	else if(key == 'l')
	{
		p->curz+=0.3;
	}
	else if(key == KEY_ESCAPE)
	{
		exit(0);
	}
	else if(key == 'z') //screenshot temp
	{
		saveScreenShot();
	}
	for(int i=0; i<targets.size(); i++)
	{
		if((targets[i].x-p->curx)*(targets[i].x-p->curx)+(targets[i].z-p->curz)*(targets[i].z-p->curz) <= 1 && targets[i].reached == false)
		{
			targets[i].reached = true;
			p->points += targets[i].points;
		}
	}
	glutPostRedisplay();
}

void Level::specialKeyPress(int key,int x,int y)
{
	if ( key == GLUT_KEY_RIGHT )
	{
		roty += 3.0;
		if(roty>=360)roty-=360;
		float tx, tz;
		tx = p->curx;
		tz = p->curz;
		p->curx = tx*cos(3.0*PI/180) + (tz + 3.4)*sin(3.0*PI/180);
		p->curz = (tz + 3.4)*cos(3.0*PI/180) - tx*sin(3.0*PI/180) - 3.4;
	}
	if ( key == GLUT_KEY_LEFT )
	{
		roty -= 3.0;
		if(roty<=0)roty+=360;
		float tx, tz;
		tx = p->curx;
		tz = p->curz;
		p->curx = tx*cos((360-3.0)*PI/180)+(tz + 3.4)*sin((360-3.0)*PI/180);
		p->curz = (tz + 3.4)*cos((360-3.0)*PI/180)-tx*sin((360-3.0)*PI/180) - 3.4;
	}
	if ( key == GLUT_KEY_UP )
	{
		p->z+=0.3;
		p->curz+=0.3*cos(roty*PI/180);
		p->curx+=0.3*sin(roty*PI/180);
		//rotx += 3.0;
		//if(rotx>=360)rotx-=360;
	}
	if ( key == GLUT_KEY_DOWN )
	{
		p->z-=0.3;
		p->curz-=0.3*cos(roty*PI/180);
		p->curx-=0.3*sin(roty*PI/180);
		//rotx -= 3.0;
		//if(rotx<=0)rotx+=360;
	}
	for(int i=0; i<targets.size(); i++)
	{
		if((targets[i].x-p->curx)*(targets[i].x-p->curx)+(targets[i].z-p->curz)*(targets[i].z-p->curz) <= 1 && targets[i].reached == false)
		{
			targets[i].reached = true;
			p->points += targets[i].points;
		}
	}
	glutPostRedisplay();
	//Handle arrow, function keys etc
}
