class Level{
	private:
	string cur_level_path;
	string cur_level;
	Model_OBJ *room;
	Model_OBJ *player;
	
	public:
	Level(int l);	
}

Level::Level(string &l)
{
	cur_level = l;
	cur_level_path = "../rooms/" + l +".obj"
	player = new Model_OBJ;
	room = new Model_OBJ;
	player.load("../models/Tux.obj")
	room.load(cur_level_path);	//&cur_level_path[0]  might avoid warning but is it safe?	

}

Level::display()
{
	
}
