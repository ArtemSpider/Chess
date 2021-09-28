#include "Controller.h"

int main()
{
	Game game(TimeControl{ 180, 2 });
	game.Run();

	return 0;
}