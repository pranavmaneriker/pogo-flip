#define PI 3.14159265359
#define MAPSIZE 20
#define BLOCKSIZE 1

#define MODE_FIRST_PERSON 1
#define MODE_THIRD_PERSON 2
#define NUM_MENU_ITEMS 3

#define JUMP_VERT_VELOCITY 0.5
#define JUMP_TIME 20
#define GRAVITY -2*JUMP_VERT_VELOCITY/(JUMP_TIME-1.0)

GLuint p1,v1,f1;
int co = 0;
int mode = 2;
int mode_dist = 20;
float mode_angle = 0;
int menu_index=0;
int show_sidehud = 1;
int hud_ox = 0;

float jump_velocity = 0;
bool is_jumping = false;
int time_spent_jumping = 0;
bool is_flipped = false;
bool show_hall = false;
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
	  if(mode == MODE_FIRST_PERSON)gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f, 1.0f, 0.0f);
	}

	void move(float i) 
	{
	  x = x + i*(lx)*0.1;
	  z = z + i*(lz)*0.1;
	  glLoadIdentity();
	  if(mode == MODE_FIRST_PERSON)gluLookAt(x, y, z, x + lx,1 + ly,z + lz, 0.0f,1.0f,0.0f);
	}
	int total_points; 
	int points, bonus;
	float velocity, acc, friction;
};

class Target{
	public:
	float x,y,z;
	int points;
	int side;	//1=top, 0=bottom
	bool reached;
	Target (float a, float b, float c, int p,int s)
	{
		x=a,y=b,z=c,points=p;
		side = s;
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
GLuint tex_2d[30] ;
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

	bool Load(const char* filename, const char* basepath = NULL, bool tex = false)
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
			glVertexPointer(3,GL_FLOAT, 0 , &(mesh.positions[0]));
			if(!mesh.normals.empty())glNormalPointer(GL_FLOAT, 0, &(mesh.normals[0]));
			
			if(!mesh.texcoords.empty())glTexCoordPointer(3,GL_FLOAT, 0, &mesh.texcoords[0]);
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &(mesh.indices[0]));		
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
		float random_angle;
		float map[MAPSIZE][MAPSIZE];
		float jump_startx, jump_startz;
	public:
		Player *p;
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
		int splash_1;
		int new0, new1, settings0, settings1, hall0, hall1, quit0, quit1, help0, help1, hud, frame;
		clock_t init;
};
//If objects with textures need loading
void
Level::initImageTextures()
{
	string path = "../textures/grass.jpg";	
	tex_grass = initImage(path);
	path = "../textures/wall.jpg";
	tex_wall = initImage(path);
	path = "../textures/abc.bmp";
	splash_1 = initImage(path);
	path = "../textures/new.bmp";
	new0= initImage(path);
	path = "../textures/new1.bmp";
	new1 = initImage(path);
	path = "../textures/hall.bmp";
	hall0 = initImage(path);
	path = "../textures/hall1.bmp";
	hall1 = initImage(path);
	path = "../textures/settings.bmp";
	settings0 = initImage(path);
	path = "../textures/settings1.bmp";
	settings1 = initImage(path);
	path = "../textures/help.bmp";
	help0 = initImage(path);
	path = "../textures/help1.bmp";
	help1 = initImage(path);
	path = "../textures/quit.bmp";
	quit0 = initImage(path);
	path = "../textures/quit1.bmp";
	quit1 = initImage(path);
	path = "../textures/hud.png";
	hud = initImage(path);
	path = "../textures/frame.png";
	frame = initImage(path);
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
	ifstream targetMap ("../Levels/1/target");
	int temp_target;	
	targets.clear();	
	for(int i=0;i<MAPSIZE;i++)
	{
		for(int j=0;j<MAPSIZE; j++)
		{
			if(targetMap.is_open())
			{
				targetMap>>temp_target;
				if(temp_target > 0)
				{
					targets.push_back(Target((i-MAPSIZE/2)*BLOCKSIZE, 0.3 , (j-MAPSIZE/2)*BLOCKSIZE, temp_target, 1));
				}
				else if(temp_target <0)
				{
					targets.push_back(Target((i-MAPSIZE/2-1)*BLOCKSIZE, 0.3 , (j-MAPSIZE/2)*BLOCKSIZE, -temp_target, 2));
				}
			}
			else
				cout<<"Couldn't open file!";
		}
	}
	levelMap.close();
	player->Load("../models/Tux.obj", "../models/");
	randomFace->Load("../models/monkey.obj","../models/");	
	g_rotation = 0;
	p = new Player;
	p->angle=PI/2;
	p->total_points=0;
	p->velocity = 0;
	p->acc = 0.05;
	p->friction = -0.1;
	p->x = 0; p->y = 1; p->z = 0; p->points =0;
	p->lx = 1; p->ly = 0; p->lz = 0;
	random_angle = 0;
	init= clock();
}

bool Level::doesCollide()
{
	float del = BLOCKSIZE/2;
	float x = p->x;
	float z = p->z;
	int i1 = floor(x/BLOCKSIZE) + MAPSIZE/2;
	int j1 = floor(z/BLOCKSIZE) + MAPSIZE/2;
	if((i1<=MAPSIZE && j1<=MAPSIZE && i1>=0 && j1>=0) && ((map[i1][j1]==0)||(is_jumping && (p->y-1 > ((is_flipped)?-map[i1][j1]*3:map[i1][j1]*3)))))
	{
		return false; //does not collide
	}
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
			if(is_flipped)
			{
				high=map[i+MAPSIZE/2][MAPSIZE-1-j-MAPSIZE/2]*3;
				if(map[i+MAPSIZE/2][MAPSIZE-1-j-MAPSIZE/2]==0)glBindTexture(GL_TEXTURE_2D,tex_grass);
				else glBindTexture(GL_TEXTURE_2D,tex_wall);
			}
			else
			{
				high=map[i+MAPSIZE/2][j+MAPSIZE/2]*3;
				if(map[i+MAPSIZE/2][j+MAPSIZE/2]==0)glBindTexture(GL_TEXTURE_2D,tex_grass);
				else glBindTexture(GL_TEXTURE_2D,tex_wall);
			}
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
				glTexCoord2f(1,0);glVertex3f(i + block, high, j + block);
				glTexCoord2f(0,0);glVertex3f(i + block, high, j);
				glTexCoord2f(0,1);glVertex3f(i + block, 0, j);
				glTexCoord2f(1,1);glVertex3f(i + block, 0, j + block);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}
}
void Level::drawTransition()
{
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
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D,splash_1);
		glBegin(GL_QUADS);
		    glTexCoord2f(0,1);glVertex2f(0.0, 0.0);
		    glTexCoord2f(1,1);glVertex2f(win.width, 0.0);
		    glTexCoord2f(1,0);glVertex2f(win.width, win.height);
		    glTexCoord2f(0,0);glVertex2f(0.0, win.height);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
		char buffer [5000];
		
		sprintf (buffer, "Level %d Completed!!\n\nBonus points :\t%d\nTotal Points :\t%d" , current_level - 1,p->bonus,  p->total_points);
		unsigned char* y;
		y = (unsigned char*) buffer;//strcat(x,rem);
		glColor3f(0,0.5,1);
		glRasterPos2i(1001, 351);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, y);
		
		glMatrixMode(GL_PROJECTION);
		
		y = (unsigned char*) buffer;//strcat(x,rem);
		glColor3f(0,0,0);
		glRasterPos2i(1000, 350);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, y);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLoadIdentity();
		glTranslatef(1.75, -1, -5);
		glScalef(0.4, 0.4, 0.4);
		glRotatef(random_angle, 0, 1, 0);
		randomFace->DrawColor();
		
		
		
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
}
void Level::nextLevel()
{
	co = 0;
	p->velocity = 0;

	clock_t final = clock()-init;
	p-> bonus = floor(400 - 10*((double)final /((double)CLOCKS_PER_SEC))) ;
	if (p->bonus < 0) p->bonus =0;
	p->total_points += p->points + ((p->bonus>0)?p->bonus:0);
	p->points = 0;
	init = clock();
	targets.clear();
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
	string target_path = "../Levels/"+str+"/target";
	ifstream targetMap (target_path.c_str());
	int temp_target;
	for(int i=0;i<MAPSIZE;i++)
	{
		for(int j=0;j<MAPSIZE; j++)
		{
			if(targetMap.is_open())
			{
				targetMap>>temp_target;
				if(temp_target > 0)
				{
					targets.push_back(Target((i-MAPSIZE/2)*BLOCKSIZE, 0.3 , (j-MAPSIZE/2)*BLOCKSIZE, temp_target, 1));
				}
				else if(temp_target <0)
				{
					targets.push_back(Target((i-MAPSIZE/2-1)*BLOCKSIZE, 0.3 , (j-MAPSIZE/2)*BLOCKSIZE, -temp_target, 2));
				}
			}
			else
				cout<<"Couldn't open file!";
		}
	}
	levelMap.close();
	g_rotation = 0;
	p->angle=0;
	p->x = 0; p->y = 1; p->z = 0; p->points =0;
}
void Level::display()
{
	if(is_transitioning)
	{
		drawTransition();
	}
	else if(show_hall)
	{
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
		
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D,splash_1);
		glBegin(GL_QUADS);
		    glTexCoord2f(0,1);glVertex2f(0.0, 0.0);
		    glTexCoord2f(1,1);glVertex2f(win.width, 0.0);
		    glTexCoord2f(1,0);glVertex2f(win.width, win.height);
		    glTexCoord2f(0,0);glVertex2f(0.0, win.height);
		glEnd();
		glBindTexture(GL_TEXTURE_2D,frame);
		glBegin(GL_QUADS);
			//glColor3f(0.7,0.9,0.4);
		    glTexCoord2f(0,1);glVertex2f(300, 300);
		    glTexCoord2f(1,1);glVertex2f(600, 300);
		    glTexCoord2f(1,0);glVertex2f(600, 600);
		    glTexCoord2f(0,0);glVertex2f(300, 600);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
		vector<int> highScores;
		int temp;
		highScores.resize(0);
		string map_path = "../hall/high";
		ifstream scores (map_path.c_str());
		while(scores>>temp)
		{
			highScores.push_back(-temp);
		}
		sort(highScores.begin(), highScores.end());
		scores.close();
		char buffer [5000];
		
		sprintf (buffer, "Top 5 Scores :\n%d\n%d\n%d\n%d\n%d", -highScores[0], -highScores[1], -highScores[2], -highScores[3], -highScores[4]);
		unsigned char* y;
		y = (unsigned char*) buffer;//strcat(x,rem);
		glColor3f(0,0,0);
		glRasterPos2i(400, 400);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, y);
		

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		
		
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLoadIdentity();
		glTranslatef(1.75, -1, -5);
		glRotatef(random_angle, 0, 1, 0);

		glUseProgram(p1);	//blinn-phong shading
		player->DrawColor();
		glUseProgram(0);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	else if(!has_started)
	{
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
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D,splash_1);
		glBegin(GL_QUADS);
		    glTexCoord2f(0,1);glVertex2f(0.0, 0.0);
		    glTexCoord2f(1,1);glVertex2f(win.width, 0.0);
		    glTexCoord2f(1,0);glVertex2f(win.width, win.height);
		    glTexCoord2f(0,0);glVertex2f(0.0, win.height);
		glEnd();
		int menu_w, menu_h, menu_gap, menu_x, menu_y;
		menu_w = 250;
		menu_h = 37;
		menu_gap = 10;
		menu_x = 300;
		menu_y = 300;
		
		for(int i=0;i<NUM_MENU_ITEMS;i++)
		{
			int temp, temp_x;
			if(menu_index==i){temp=i+NUM_MENU_ITEMS;temp_x = menu_x + 20;}
			else{temp_x = menu_x; temp = i;}
			glColor3f(1,1,1);
			if(temp==0) glBindTexture(GL_TEXTURE_2D,new0);
			else if(temp==1) glBindTexture(GL_TEXTURE_2D,hall0);
			else if(temp==2) glBindTexture(GL_TEXTURE_2D,quit0);
			else if(temp==3) glBindTexture(GL_TEXTURE_2D,new1);
			else if(temp==4) glBindTexture(GL_TEXTURE_2D,hall1);
			else if(temp==5) glBindTexture(GL_TEXTURE_2D,quit1);

			glBegin(GL_QUADS);
				glTexCoord2f(0,0);glVertex2f(temp_x, menu_y + i*(menu_gap+menu_h) + menu_h);
				glTexCoord2f(0,1);glVertex2f(temp_x, menu_y + i*(menu_gap+menu_h));
				glTexCoord2f(1,1);glVertex2f(temp_x + menu_w,  menu_y + i*(menu_gap+menu_h));
				glTexCoord2f(1,0);glVertex2f(temp_x + menu_w, menu_y + i*(menu_gap+menu_h) + menu_h);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		
		
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLoadIdentity();
		glTranslatef(1.75, -1, -5);
		glRotatef(random_angle, 0, 1, 0);

		glUseProgram(p1);	//blinn-phong shading
		player->DrawColor();
		glUseProgram(0);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		if(p->velocity >= -p->friction)
			p->velocity += p->friction;
		else if(p->velocity <= p->friction)
			p->velocity -= p->friction;
		p->move(p->velocity);
		if(doesCollide())
		{
			p->move(-p->velocity);
			p->velocity = 0;
			p->acc = 0.5;	
		}
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
		
		if(mode == MODE_FIRST_PERSON)
		{
			gluLookAt(p->x, p->y, p->z, p->x + p->lx,p->y + p->ly,p->z + p->lz, 0.0f, 1.0f, 0.0f);
		}		
		else if(mode == MODE_THIRD_PERSON)
		{
			gluLookAt(p->x+(mode_dist/2)*cos((mode_angle+45)*PI/180), mode_dist, p->z+(mode_dist/2)*sin((mode_angle+45)*PI/180), p->x,p->y,p->z, -0.57f, 0.57f, -0.57f); //change the up vector
		}
		
		if(is_jumping)
		{
			p->y += jump_velocity;
			float dist = 3.0/JUMP_TIME;
			p->x = p->x + dist*(p->lx);
	  		p->z = p->z + dist*(p->lz);
			time_spent_jumping ++;
			jump_velocity += GRAVITY;
			if(doesCollide())
			{
				time_spent_jumping = JUMP_TIME;
				p->x = jump_startx;
	  			p->z = jump_startz;
	  			p->y = 1;
			}
		}
		glPushMatrix();
			glTranslatef(p->x,p->y+-1,p->z);
			//glTranslatef(p->lx,p->ly,p->lz);
			glRotatef(-(p->angle*180/3.14),0,1,0);
			glRotatef(180,0,1,0);
			//glScalef(0.5,0.5,0.5);
			glUseProgram(p1);	//blinn-phong shading
			player->DrawColor();
			glUseProgram(0);
		glPopMatrix();
		if(time_spent_jumping >= JUMP_TIME)
		{
			is_jumping = false;
			p->y = 1;
		}
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
		
		int hud_width = 15*win.width/100 + 20;
		int hud_pad = 15;
		
		if(show_sidehud)
		{
			hud_ox+=20;
			hud_ox = min(0, hud_ox);
		}
		else
		{
			hud_ox-=20;
			hud_ox = max(-hud_width, hud_ox);
		}
		glEnable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D,hud);
		glBegin(GL_QUADS);
		    glTexCoord2f(0,1);glVertex2f(hud_ox + 0.0, 0.0);
		    glTexCoord2f(1,1);glVertex2f(hud_ox + hud_width, 0.0);
		    glTexCoord2f(1,0);glVertex2f(hud_ox + hud_width, win.height);
		    glTexCoord2f(0,0);glVertex2f(hud_ox + 0.0, win.height);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		int bar_width = 500;
		int bar_height = 50;
		int bar_pad = 10;
		float sh=3;
		hud_pad = 0;
		glBegin(GL_QUADS);
		    glColor3f(0,0,0.05);
		    glVertex2f(win.width-hud_pad-bar_width+sh, hud_pad+sh);
		    glVertex2f(win.width-hud_pad+sh, hud_pad+sh);
		    glVertex2f(win.width-hud_pad+sh, hud_pad+bar_height+sh);
		    glVertex2f(win.width-hud_pad-bar_width+sh, hud_pad+bar_height+sh);
		glEnd();
		glBegin(GL_QUADS);
		    glColor3f(0,0,0.6);
		    glVertex2f(win.width-hud_pad-bar_width-25, hud_pad);
		    glVertex2f(win.width-hud_pad, hud_pad);
		    glVertex2f(win.width-hud_pad, hud_pad+bar_height);
		    glVertex2f(win.width-hud_pad-bar_width, hud_pad+bar_height);
		glEnd();
		glBegin(GL_QUADS);
		    glColor3f(0,0.5,1);
		    glVertex2f(win.width-hud_pad-bar_width+bar_pad, hud_pad+bar_pad);
		    glVertex2f(win.width-hud_pad-bar_pad, hud_pad+bar_pad);
		    glVertex2f(win.width-hud_pad-bar_pad, hud_pad+bar_height-bar_pad);
		    glVertex2f(win.width-hud_pad-bar_width+bar_pad, hud_pad+bar_height-bar_pad);
		glEnd();

		int total_targets=3;
		float bar_length = co*(bar_width-2*bar_pad)/targets.size();
		glBegin(GL_QUADS);
		    glColor3f(1,1,0);
		    glVertex2f(win.width-hud_pad-bar_width+bar_pad, hud_pad+bar_pad);
		    glColor3f(1,1,0);
		    glVertex2f(win.width-hud_pad+bar_pad+bar_length-bar_width, hud_pad+bar_pad);
		    glColor3f(1,0.4,0);
		    glVertex2f(win.width-hud_pad+bar_pad+bar_length-bar_width, hud_pad+bar_height-bar_pad);
		    glColor3f(1,0.4,0);
		    glVertex2f(win.width-hud_pad-bar_width+bar_pad, hud_pad+bar_height-bar_pad);
		glEnd();
		
		//bonus bar
		hud_pad = bar_height+sh;
		bar_width = 400;
		bar_height = 40;
		bar_pad = 8;
		sh=3;
		glBegin(GL_QUADS);
		    glColor3f(0,0,0);
		    glVertex2f(win.width-bar_width+sh , hud_pad+sh);
		    glVertex2f(win.width+sh, hud_pad+sh);
		    glVertex2f(win.width+sh, hud_pad+bar_height+sh);
		    glVertex2f(win.width-bar_width+sh, hud_pad+bar_height+sh);
		glEnd();
		glBegin(GL_QUADS);
		    glColor3f(0.4, 0.4, 0.4);
		    glVertex2f(win.width-bar_width-20, hud_pad);
		    glVertex2f(win.width, hud_pad);
		    glVertex2f(win.width, hud_pad+bar_height);
		    glVertex2f(win.width-bar_width, hud_pad+bar_height);
		glEnd();
		glBegin(GL_QUADS);
		    glColor3f(0.6,0.6, 0.6);
		    glVertex2f(win.width-bar_width+bar_pad, hud_pad+bar_pad);
		    glVertex2f(win.width-bar_pad, hud_pad+bar_pad);
		    glVertex2f(win.width-bar_pad, hud_pad+bar_height-bar_pad);
		    glVertex2f(win.width-bar_width+bar_pad, hud_pad+bar_height-bar_pad);
		glEnd();
		clock_t final = clock()-init;
		bar_length = bar_width - 10*((double)final /((double)CLOCKS_PER_SEC));
		if(bar_length<0)bar_length=0;
		glBegin(GL_QUADS);
		    glColor3f(1,1,1);
		    glVertex2f(win.width-bar_width+bar_pad, hud_pad+bar_pad);
		    glColor3f(1,1,1);
		    glVertex2f(win.width+bar_pad+bar_length-bar_width, hud_pad+bar_pad);
		    glColor3f(0.5,0.5,1);
		    glVertex2f(win.width+bar_pad+bar_length-bar_width, hud_pad+bar_height-bar_pad);
		    glColor3f(0.5,0.5,1);
		    glVertex2f(win.width-bar_width+bar_pad, hud_pad+bar_height-bar_pad);
		glEnd();
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
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double radius = 115;   
		glColor4f(1, 1, 1, 0.2);
		double twicePi = 2.0 * 3.142;
		glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
		glVertex2f(map_ox, map_oy); // center of circle
		for (int i = 0; i <= 40; i++)   {
			glVertex2f ( (map_ox + (radius * cos(i * twicePi / 40))), (map_oy + (radius * sin(i * twicePi / 40))) );
		}
		glEnd();
		
		radius = 110;   
		
		glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
		glColor4f(0.85, 1, 0, 0.1);
		glVertex2f(map_ox, map_oy); // center of circle
		for (int i = 0; i <= 40; i++)   {
			glColor4f(0.28, 1, 0, 0.7);
			glVertex2f ( (map_ox + (radius *cos(i * twicePi / 40))), (map_oy + (radius * sin(i * twicePi / 40))) );
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
						glColor4f(0,0.8,0,0);
					glVertex2f(le, bo);
					glVertex2f(le+unit, bo);
					glVertex2f(le+unit, bo -unit);
					glVertex2f(le, bo-unit);
				glEnd();
			}
		}
		glColor3f(0,0,0);
		glPointSize(1);
		glBegin(GL_LINES);
		    glVertex3f(map_ox - p->z*unit, map_oy + p->x*unit ,0);
		    glVertex3f(map_ox - p->z*unit -p->lz*4*unit, map_oy + p->x*unit + p->lx*4*unit,0);
		glEnd();
		int point_size = unit + abs(6 * sin(random_angle*PI/180));
		glPointSize(point_size);
		
		glBegin(GL_POINTS);
			//tux
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(map_ox - p->z*unit , map_oy + p->x*unit ,0);
			glColor3f(0,0.5,1);
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
		glDisable(GL_BLEND);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	glutSwapBuffers();
	alListener3f(AL_POSITION,p->x,p->y,p->z);
	
	
	if(co == targets.size())
		nextLevel();
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
		if(!is_jumping)
		{
			if( key == 'w' || key == 'W')
			{
				p->acc = 0.5/(abs(p->velocity)+1);
				p->velocity += p->acc;
				// p->move(1);
				// if(doesCollide())
				// {
				// 	p->move(-1);
				// }
			}
			else if( key == 's' || key == 'S')
			{
				p->acc = 0.5/(abs(p->velocity)+1);
				p->velocity -= p->acc;
				// p->move(-1);
				// if(doesCollide())
				// {
				// 	p->move(1);
				// }
			}
			else if(key == 'a' || key == 'A')
			{
				p->angle -= 0.1f;p->orient(p->angle);
				if(doesCollide())
				{
					p->angle += 0.1f;p->orient(p->angle);
				}
			}
			else if(key == 'd' || key == 'D')
			{
				p->angle +=0.1f;p->orient(p->angle);
				if(doesCollide())
				{
					p->angle -=0.1f;p->orient(p->angle);
				}
			}
			else if(key== 'e' || key=='E')
			{	
				is_jumping = true;
				time_spent_jumping = 0;
				jump_velocity = JUMP_VERT_VELOCITY;
				jump_startx = p->x;
				jump_startz = p->z;
			}
		}
		if(key == 'q' || key == 'Q')
		{
			//p->z *= -1;
			
			//p->angle = 3*PI-p->angle;
			//p->orient(p->angle);
			is_flipped = 1 - is_flipped;
			glutTimerFunc(100, rotate , 1 );
		}
		else if(key == 'o')
		{
			nextLevel();
		}
		else if(key == 'm' || key=='M')
		{
			if(mode == MODE_FIRST_PERSON) mode = MODE_THIRD_PERSON;
			else if( mode == MODE_THIRD_PERSON) mode = MODE_FIRST_PERSON;
		}
		else if(key == 'z')
		{
			mode_dist += 2;
		}
		else if(key == 'x')
		{
			mode_dist -= 2;
		}
		else if(key == 'c')
		{
			mode_angle += 5;
		}
		else if(key == 'v')
		{
			mode_angle -= 5;
		}
		else if(key== 'b' )
		{	
			show_sidehud = 1 - show_sidehud;
		}
		else if(key== 'n' )
		{	
			p->x=p->z=0;p->y=1;
		}
	}
	if(key == 'y')
	{
		has_started = true;
		init = clock();
		is_transitioning = false;
	}
	else if(key == 'r')
	{
		saveScreenShot();
	}
	else if(key == ' ')
	{
		if(menu_index == 0)
			has_started = true;
		if(menu_index == 1)
			show_hall = true;
		if(menu_index == 2)
		{
			//write score to a file
			string map_path = "../hall/high";
			ofstream scores (map_path.c_str(),ios::out | ios::app );
			scores<<endl<<p->points;
			scores.close();
			exit(0);
		}
	}
	else if(key=='p')
	{
		has_started = 1-has_started;
		show_hall = false;
	}
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
////////}
////////if ( key == GLUT_KEY_LEFT )
////////{
////////}
if ( key == GLUT_KEY_UP )
{
	if(!has_started)
	{
		menu_index = (menu_index-1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
	}
}
if ( key == GLUT_KEY_DOWN )
{
	if(!has_started)
	{
		menu_index = (menu_index+1) % NUM_MENU_ITEMS;
	}
}
glutPostRedisplay();
}
