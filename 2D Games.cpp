#include <iostream>
#include "Game.h"

//using namespace sf;

int main()
{
	//init srand
	std::srand(static_cast<unsigned>(time(NULL)));

	//Init Game Engine
	Game game;

	//Game loop
	while (game.getWindowIsOpen() && !game.getEndGame())
	{
		//Update
		game.update();
		
		//Render
		game.render();
	}

	//End of application
	return 0;
}