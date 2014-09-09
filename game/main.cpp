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
	void start_level();
	Level level;
	glWindow window;
	
	public:
	Game();
	void start();
	
}

Game::Game()
{
	Level level = new Level(1);
}

Game::start()
{
	initGl();
	start_level(level);
}



int main()
{
	Game *g = new Game;
	Game->start();
	return 0;
}
