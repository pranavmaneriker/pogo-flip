#define PI 3.14159265359
#define MAPSIZE 20
#define BLOCKSIZE 1
float rotx,roty,rotz;
GLuint p1,v1,f1;
int co = 0;

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

void saveScreenShot()
{
	int save_result = SOIL_save_screenshot
	(
		"screenshot.bmp",
		SOIL_SAVE_TYPE_BMP,
		0, 0, 1024, 768
	);
}

//Global textures
GLuint tex_2d[10] ;
int lastUsed=0;
int initImage(string path)
{
	tex_2d[lastUsed] = SOIL_load_OGL_texture
	(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);
	if( 0 == tex_2d[lastUsed] )
	{	
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	}
	    glBindTexture(GL_TEXTURE_2D, tex_2d[lastUsed]);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	return tex_2d[lastUsed++];
	
}

/**
* Handling of in game objects
**/
class Model_OBJ{
	public:
	Model_OBJ()
	{		
	}
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	map<string,GLuint> texmaps;
	string mtlpath;

	bool LoadTextures()
	{
		tinyobj::material_t mat;
		string path;
		for(unsigned int i =0 ; i<shapes.size();++i)
		{
			if(!shapes[i].mesh.material_ids.empty())
			{
				mat = materials[shapes[i].mesh.material_ids[0]];	//assuming one material per shape
				if(!mat.diffuse_texname.empty())
				{
					cout<<"Loading texture: "<<mat.diffuse_texname<<endl;
					path = mtlpath+mat.diffuse_texname;
					GLuint tex = SOIL_load_OGL_texture(
						path.c_str(),
						SOIL_LOAD_AUTO,
						SOIL_CREATE_NEW_ID,
						SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
					);
					cout<<"Loaded"<<endl;
					if( 0 == tex )
					{	
						printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
					}

					texmaps[mat.name] = tex;
				}
			}
		}
		return true;
	}

	bool Load(const char* filename, const char* basepath = NULL)
	{
	  std::cout << "Loading " << filename << std::endl;
	  string err = tinyobj::LoadObj(shapes, materials, filename, basepath);
	  if (!err.empty()) {
	    std::cerr << err << std::endl;
	    return false;
	  }
	  mtlpath = basepath;
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
			if(!mesh.normals.empty())glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));	
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
		//	glBindTexture(GL_TEXTURE_2D, texmaps[materials[mat].name]);	
			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			if(!mesh.normals.empty())glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));
			
			if(!mesh.texcoords.empty())glTexCoordPointer(3,GL_FLOAT, 0, &mesh.texcoords[0]);
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
	int current_level;
	Model_OBJ *room;
	Model_OBJ *inv;
	Model_OBJ *player;
	Model_OBJ *randomFace;
	int g_rotation;
	Player *p;
	float random_angle;
	float map[MAPSIZE][MAPSIZE];
	public:
	bool has_started;
	bool is_transitioning;
	vector<Target> targets;
	Level(string &l);
	void display();
	void keyPress(unsigned char ch, int x, int y);
	void specialKeyPress(int key, int x, int y);
	void rotateFace();
	void nextLevel();
	void initImageTextures();
	void drawTransition();
	void drawTerrain();
	bool doesCollide();
	int tex_grass;
	int tex_wall;
};
//If objects with textures need loading
void
Level::initImageTextures()
{
	//inv->LoadTextures();
	string path = "../textures/grass.jpg";	
	tex_grass = initImage(path);
	path = "../textures/wall.jpg";
	tex_wall = initImage(path);	
}
//Loading level 
Level::Level(string &l)
{
	has_started = false;
	is_transitioning = false;
	flip_angle = 0;
	cur_level = l;
	current_level = 1;
	cur_level_path = "../rooms/" + l +"_alt.obj";
	inv = new Model_OBJ;
	player = new Model_OBJ;
	room = new Model_OBJ;
	randomFace = new Model_OBJ;
	//inv->Load("../rooms/Level_1_test.obj","../rooms/");
	ifstream levelMap ("../Levels/1/map");
	for(int i=0;i<MAPSIZE;i++)
	{
		for(int j=0;j<MAPSIZE; j++)
		{
			if(levelMap.is_open())
			{
				levelMap>>map[i][j];
			}
			else
				cout<<"Couldn't open file!";
		}
	}
	levelMap.close();
	//inv->Load("../rooms/texture/texture.obj", "../rooms/texture/");
	//inv->Load("../rooms/Small Tropical Island/Small Tropical Island.obj", "../rooms/Small Tropical Island/");
	//inv->Load("../rooms/SnowTerrain/SnowTerrain.obj", "../rooms/SnowTerrain/");
	player->Load("../models/Tux.obj", "../models/");
	randomFace->Load("../models/monkey.obj","../models/");
	//room->Load(&cur_level_path[0],"../rooms/");	//&cur_level_path[0]  might avoid warning but is it safe?	
	g_rotation = 0;
	p = new Player;
	p->angle=0;
	p->x = 0; p->y = 1; p->z = 0; p->points =0;
	roty=54;
//	p->curx=p->cury=0; p->curz=3.4;	
	p->lx = 1; p->ly = 0; p->lz = 0;
	random_angle = 0;
	targets.push_back(Target(5, 0.2 , 2, 20));
	targets.push_back(Target(7, 0.2 , -4, 30));
	targets.push_back(Target(-2, 0.2 , 3, 50));
	
}

bool Level::doesCollide()
{
	float del = BLOCKSIZE/2;
	float x = p->x+p->lx - del;
	float z = p->z+p->lz - del;
	int i1 = floor(x/BLOCKSIZE) + MAPSIZE/2 +1;
	int j1 = floor(z/BLOCKSIZE) + MAPSIZE/2;
	x += 2*del;
	z += 2*del;
	int i2 = floor(x/BLOCKSIZE) + MAPSIZE/2;
	int j2 = floor(z/BLOCKSIZE) + MAPSIZE/2;
	if(i1<=MAPSIZE && j1<=MAPSIZE && map[i1][j1]==0
		&& i1<=MAPSIZE && j2<=MAPSIZE && map[i1][j2]==0
		&& i2<=MAPSIZE && j1<=MAPSIZE && map[i2][j1]==0
		&& i2<=MAPSIZE && j2<=MAPSIZE && map[i2][j2]==0
		&& i1>=0 && i2>=0 && j1>=0 && j2>=0)
		return false; //does not collide
	else
		return true; //does collide
}


void Level::rotateFace()
{
	random_angle +=3;
	if(random_angle >=360) random_angle-=360;
}
/**
* Calculate normals with cross product
**/



/**
* Level scene drawing function
* Contains welcome screen also
**/
void Level::drawTerrain()
{
	float high;
	float block = (float)BLOCKSIZE;
	for(int i=-MAPSIZE/2; i<MAPSIZE/2; i+=1)
	{
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		for(int j=-MAPSIZE/2; j<MAPSIZE/2;j+=1)
		{
			glBindTexture(GL_TEXTURE_2D,tex_grass);
			
			high=map[i+MAPSIZE/2][j+MAPSIZE/2]*3;
			glBegin(GL_QUADS);
				glNormal3f(0,-1,0);
				glTexCoord2f(0,0);glVertex3f(i , high, j + block);
				glTexCoord2f(0,1);glVertex3f(i + block, high, j + block);
				glTexCoord2f(1,1);glVertex3f(i + block, high, j);
				glTexCoord2f(1,0);glVertex3f(i, high, j);
			glEnd();
			glBindTexture(GL_TEXTURE_2D,tex_wall);
			glBegin(GL_QUADS);
				glNormal3f(0,0,1);
				glTexCoord2f(1,0);glVertex3f(i, high, j + block);
				glTexCoord2f(0,0);glVertex3f(i + block, high, j + block);
				glTexCoord2f(0,1);glVertex3f(i + block, 0, j + block);
				glTexCoord2f(1,1);glVertex3f(i, 0, j + block);
			glEnd();
			glBegin(GL_QUADS);
					glNormal3f(1,0,0);
					glTexCoord2f(1,0);glVertex3f(i, high, j + block);
					glTexCoord2f(0,0);glVertex3f(i, high, j);
					glTexCoord2f(0,1);glVertex3f(i, 0, j);
					glTexCoord2f(1,1);glVertex3f(i, 0, j + block);
			glEnd();
			glBegin(GL_QUADS);
				glNormal3f(0,0,1);
				glTexCoord2f(1,0);glVertex3f(i + block, high, j);
				glTexCoord2f(0,0);glVertex3f(i, high, j);
				glTexCoord2f(0,1);glVertex3f(i, 0, j);
				glTexCoord2f(1,1);glVertex3f(i + block, 0, j);
			glEnd();
			glBegin(GL_QUADS);
				glNormal3f(1,0,0);
				glTexCoord2f(0,0);glVertex3f(i + block, high, j + block);
				glTexCoord2f(0,1);glVertex3f(i + block, high, j);
				glTexCoord2f(1,1);glVertex3f(i + block, 0, j);
				glTexCoord2f(1,0);glVertex3f(i + block, 0, j + block);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	//inv->Load("../rooms/Level_1_test.obj","../rooms/");
	//inv->Load("../rooms/texture/texture.obj", "../rooms/texture/");
	//inv->Load("../rooms/SnowTerrain/SnowTerrain.obj", "../rooms/SnowTerrain/");
	}
}
void Level::drawTransition()
{
	glClearColor(0.7215,0.8627,0.9490,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLoadIdentity();
	glTranslatef(0, -1, -3);
	glRotatef(random_angle, 0, 1, 0);
	player->DrawColor();
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
	const char* x = "                Pogo Flip\n_______________________";
	unsigned char* y;
	y = (unsigned char*) x;
	glColor3f(0,0,0);
	glRasterPos2i(win.width/2 - 150, win.height/4);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, y);
	x = "Press 'y' to continue";
	y = (unsigned char*) x;
	glColor3f(0,0,0);
	glRasterPos2i(win.width/2 - 75, win.height/4 +75);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, y);
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void Level::nextLevel()
{
	is_transitioning = true;
	current_level++;
	flip_angle = 0;
	stringstream ss;
	ss << current_level;
	string str = ss.str();
	string map_path = "../Levels/"+str+"/map";
	ifstream levelMap (map_path.c_str());
	for(int i=0;i<MAPSIZE;i++)
	{
		for(int j=0;j<MAPSIZE; j++)
		{
			if(levelMap.is_open())
			{
				levelMap>>map[i][j];
			}
			else
				cout<<"Couldn't open file!";
		}
	}
	levelMap.close();
	g_rotation = 0;
	p->angle=0;
	p->x = 0; p->y = 1; p->z = 0; p->points =0;
	roty=54;
}
void Level::display()
{

	glUseProgram(p1);	//blinn-phong shading
	if(is_transitioning)
	{
		drawTransition();
	}
	else if(!has_started)
	{
		glClearColor(0.7215,0.8627,0.9490,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLoadIdentity();
		glTranslatef(0, -1, -3);
		glRotatef(random_angle, 0, 1, 0);
		player->DrawColor();
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
		const char* x = "                Pogo Flip\n_______________________";
		unsigned char* y;
		y = (unsigned char*) x;
		glColor3f(0,0,0);
		glRasterPos2i(win.width/2 - 150, win.height/4);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, y);
		x = "Press 'y' to start the game.";
		y = (unsigned char*) x;
		glColor3f(0,0,0);
		glRasterPos2i(win.width/2 - 75, win.height/4 +75);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, y);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		glClearColor(0.7215,0.8627,0.9490,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		
		glLoadIdentity();
		float pos[]={0,1,1};
		glLightfv(GL_LIGHT0,GL_POSITION,pos);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 100.0);
    		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);
		//float dir[]={p->lx,p->ly,p->lz};
		float dir[]={0,0,-1};
		glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,dir);
		glEnable(GL_LIGHT0);
		gluLookAt(p->x, p->y, p->z, p->x + p->lx,p->y + p->ly,p->z + p->lz, 0.0f, 1.0f, 0.0f);
		

		glPushMatrix();
			glTranslatef(p->x,p->y+-1,p->z);
			glTranslatef(p->lx,p->ly,p->lz);
			glRotatef(-(p->angle*180/3.14),0,1,0);
			glTranslatef(0,0,-0.5);
			glRotatef(180,0,1,0);
			//glScalef(0.5,0.5,0.5);
			//player->DrawColor();
		glPopMatrix();
	//	glRotatef(roty, 0, 1, 0);
	//	glRotatef(rotx, 1, 0, 0);


		glRotatef(flip_angle,1,0,0);
		
		glPushMatrix();
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
			//glEnable(GL_COLOR_MATERIAL);
			glDisable(GL_LIGHTING);
			drawTerrain();

			//room->DrawColor();
			glEnable(GL_LIGHTING);
		glPopMatrix();
		for(int i = 0; i < targets.size() ; i++)
		{
			if(targets[i].reached==false)
			{
				glPushMatrix();
					glTranslatef(targets[i].x,targets[i].y,targets[i].z);
					glRotatef(random_angle, 0,1,0);
					glScalef(0.25,0.25,0.25);
					randomFace->DrawColor();
				glPopMatrix();
			}
		}
		glTranslatef(0, -0.02,0);
		glRotatef(180,1,0,0);

		glPushMatrix();
			//glTranslatef(0,-50,0);
			glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping
			//inv->DrawColor();
			//inv->DrawTexture();
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	
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
		
		int hud_width = 15*win.width/100;
		int hud_pad = 15;
		glBegin(GL_QUADS);
		    glColor3f(0.8, 0.4, 0.4);
		    glVertex2f(0.0, 0.0);
		    glVertex2f(hud_width, 0.0);
		    glVertex2f(hud_width, win.height);
		    glVertex2f(0.0, win.height);
		glEnd();
		
		//printing text
		char buffer [5000];
		
		sprintf (buffer, "Pogo Flip\n----------------\nReach targets using \na,w,s,d keys \nto score points \nUse r to take \nscreenshots.\n\nMonkeys to go: %d\n\n\nPoints : %d" , targets.size()-co, p->points);
		unsigned char* y;
		y = (unsigned char*) buffer;//strcat(x,rem);
		glColor3f(0,0,0);
		glRasterPos2i(hud_pad, 100);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, y);
		
		/*
		glBegin(GL_QUADS);
		    glColor3f(0, 1, 0);
		    glVertex2f(hud_pad, win.height+hud_pad-hud_width);
		    glVertex2f(hud_width - hud_pad, win.height+hud_pad-hud_width);
		    glVertex2f(hud_width - hud_pad, win.height-hud_pad);
		    glVertex2f(hud_pad, win.height - hud_pad);
		glEnd();
		*/
		glDisable(GL_PROGRAM_POINT_SIZE);
		
		
		//lookat arrow
		int map_ox = hud_width/2;
		int map_oy = win.height - hud_width/2;
		//glRotatef(random_angle, 0, 0, 1);
		//glTranslatef(map_ox, map_oy, 0);
		
		double radius = 150;   
		glColor3f(0, 0.8, 0);
		double twicePi = 2.0 * 3.142;
		glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
		glVertex2f(map_ox, map_oy); // center of circle
		for (int i = 0; i <= 40; i++)   {
			glVertex2f ( (map_ox + (radius * cos(i * twicePi / 40))), (map_oy + (radius * sin(i * twicePi / 40))) );
		}
		glEnd();
		
		radius = 130;   
		glColor3f(0, 1, 0);
		glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
		glVertex2f(map_ox, map_oy); // center of circle
		for (int i = 0; i <= 40; i++)   {
			glVertex2f ( (map_ox + (radius * cos(i * twicePi / 40))), (map_oy + (radius * sin(i * twicePi / 40))) );
		}
		glEnd();
		
		//the map
		
		float face_angle = -atan(p->lz/p->lx)*180/PI + 180;
		if((p->lz>=0 && p->lx<=0)||(p->lz<=0 && p->lx<=0))face_angle+=180;
		
		
		float unit = 7;
		glTranslatef(map_ox, map_oy, 0);
		glRotatef(face_angle, 0, 0, 1);
		map_ox=map_oy=0;
		for(int i=-MAPSIZE/2; i<MAPSIZE/2; i+=1)
		{
			for(int j=-MAPSIZE/2; j<MAPSIZE/2;j+=1)
			{
				float high = map[i+MAPSIZE/2][j+MAPSIZE/2];
				float le, bo;
					le = map_ox - j*unit;
					bo = map_oy + i*unit;
				glBegin(GL_QUADS);
					if(high>0)
						glColor3f(0.8, 0.411, 0.12);
					else if(high<0)
						glColor3f(0,0,0);
					else
						glColor3f(0,1,0);
					glVertex2f(le, bo);
					glVertex2f(le, bo - unit);
					glVertex2f(le - unit, bo - unit);
					glVertex2f(le - unit, bo);
				glEnd();
			}
		}
		glColor3f(0,0,0);
		glPointSize(1);
		glBegin(GL_LINES);
		    glVertex3f(map_ox - p->z*unit -p->lz*unit, map_oy + p->x*unit + p->lx*unit,0);
		    glVertex3f(map_ox - p->z*unit -p->lz*4*unit, map_oy + p->x*unit + p->lx*4*unit,0);
		glEnd();
		int point_size = unit + abs(6 * sin(random_angle*PI/180));
		glPointSize(point_size);
		
		glBegin(GL_POINTS);
			//tux
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(map_ox - p->z*unit - p->lz*unit , map_oy + p->x*unit + p->lx*unit ,0);
			glColor3f(1,1,0);
			//targets
			for(int i=0;i<targets.size();i++)
			{
				if(targets[i].reached!=true)
				{
					glVertex3f(map_ox - targets[i].z*unit , map_oy + targets[i].x*unit,0);
				}
			}
		glEnd();
		
		// Making sure we can render 3d again
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	glutSwapBuffers();	
	//glFlush(); 
	alListener3f(AL_POSITION,p->x,p->y,p->z);
	//cout<<rotx<<" "<<roty<<" "<<rotz<<" "<<p->x<<" "<<p->y<<" "<<p->z<<endl; 
}

/**
* Idle function: Rotate monkey head.
**/
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

/**
* Key events handling
**/
void Level::keyPress(unsigned char key, int x, int y)
{
	//normal key press events
	if(has_started)
	{
		if( key == 'w' || key == 'W')
		{
			p->move(1);
			if(doesCollide())
			{
				p->move(-1);
			}
			//p->z+=0.3;
		}
		else if( key == 's' || key == 'S')
		{
			p->move(-1);
			if(doesCollide())
			{
				p->move(1);
			}
			//p->z-=0.3;
		}
		else if(key == 'a' || key == 'A')
		{
			p->angle -= 0.1f;p->orient(p->angle);
			if(doesCollide())
			{
				p->angle += 0.1f;p->orient(p->angle);
			}
			//p->x+=0.3;
		}
		else if(key == 'd' || key == 'D')
		{
			p->angle +=0.1f;p->orient(p->angle);
			if(doesCollide())
			{
				p->angle -=0.1f;p->orient(p->angle);
			}
		//	p->x-=0.3;
		}
		else if(key == 'q' || key == 'Q')
		{
			glutTimerFunc(100, rotate , 1 );
		}
		else if(key == 'o')
		{
			nextLevel();
		}
	}
	if(key == 'y')
	{
		has_started = true;
		is_transitioning = false;
	}
	else if(key == 'r')
	{
		saveScreenShot();
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

	for(int i=0; i<targets.size(); i++)
	{
		if((targets[i].x-(p->x+p->lx))*(targets[i].x-(p->x+p->lx))+(targets[i].z-(p->lz+p->z))*(targets[i].z-(p->lz+p->z)) <= 1 && targets[i].reached == false)
		{
			targets[i].reached = true;
			p->points += targets[i].points;
			co++;
		}
	}
	glutPostRedisplay();
}

/**
* Special key events handling. Currently disabled
**/

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
