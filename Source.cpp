#include <iostream>
#include <conio.h>
#include <string>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>

#include "Coords.h"
#include "Other.h"
#include "Piece.h"
#include "Board.h"
#include "Pieces.h"

using namespace std;


ChessBoard* CreateBoard()
{
	ChessBoard* res = new ChessBoard({ 8, 8 });

	vector<vector<Piece*> > grid(8, vector<Piece*>(8, nullptr));
	for (int j = 0; j < 8; j++)
	{
		grid[1][j] = new Pawn({ 1, j }, PlayerTeam::White, res);
		grid[6][j] = new Pawn({ 1, j }, PlayerTeam::Black, res);
	}

	//grid[0][0] = new Rook({ 0, 0 }, PlayerTeam::White, res);
	//grid[7][0] = new Rook({ 7, 0 }, PlayerTeam::Black, res);

	grid[0][1] = new Knight({ 0, 1 }, PlayerTeam::White, res);
	grid[7][1] = new Knight({ 7, 1 }, PlayerTeam::Black, res);

	//grid[0][0] = new Bishop({ 0, 2 }, PlayerTeam::White, res);
	//grid[7][0] = new Bishop({ 7, 2 }, PlayerTeam::Black, res);

	//grid[0][0] = new Queen({ 0, 3 }, PlayerTeam::White, res);
	//grid[7][0] = new Queen({ 7, 3 }, PlayerTeam::Black, res);

	//grid[0][0] = new King({ 0, 4 }, PlayerTeam::White, res);
	//grid[7][0] = new King({ 7, 4 }, PlayerTeam::Black, res);

	//grid[0][0] = new Bishop({ 0, 5 }, PlayerTeam::White, res);
	//grid[7][0] = new Bishop({ 7, 5 }, PlayerTeam::Black, res);

	grid[0][0] = new Knight({ 0, 6 }, PlayerTeam::White, res);
	grid[7][0] = new Knight({ 7, 6 }, PlayerTeam::Black, res);

	//grid[0][0] = new Rook({ 0, 7 }, PlayerTeam::White, res);
	//grid[7][0] = new Rook({ 7, 7 }, PlayerTeam::Black, res);

	return res;
}

ChessBoard* CreateBoard960()
{
	// TODO
	return nullptr;
}


int main()
{
	srand(time(0));





	_getch();
	return 0;
}