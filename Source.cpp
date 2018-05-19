#include "Game.h"

int main()
{
	Game game;
	if (!game.init())
		return EXIT_FAILURE;
	return game.exec();
}