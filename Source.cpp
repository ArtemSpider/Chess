#include <iostream>
#include <conio.h>
#include <string>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>

#include "Chess.h"

using namespace std;


int main()
{
	srand(time(0));


	Game game;
	game.Run();


	return 0;
}