#pragma once

#include "Board.h"
#include "Pieces.h"


ChessBoard* CreateBoard(TimeControl timeControl)
{
	ChessBoard* res = new ChessBoard(Size(8, 8), timeControl);

	vector<vector<Piece*> > grid(8, vector<Piece*>(8, nullptr));
	for (int j = 0; j < 8; j++)
	{
		grid[1][j] = new Pawn({ j, 1 }, PlayerTeam::White, res);
		grid[6][j] = new Pawn({ j, 6 }, PlayerTeam::Black, res);
	}

	grid[0][0] = new Rook({ 0, 0 }, PlayerTeam::White, res);
	grid[7][0] = new Rook({ 0, 7 }, PlayerTeam::Black, res);

	grid[0][1] = new Knight({ 1, 0 }, PlayerTeam::White, res);
	grid[7][1] = new Knight({ 1, 7 }, PlayerTeam::Black, res);

	grid[0][2] = new Bishop({ 2, 0 }, PlayerTeam::White, res);
	grid[7][2] = new Bishop({ 2, 7 }, PlayerTeam::Black, res);

	grid[0][3] = new Queen({ 3, 0 }, PlayerTeam::White, res);
	grid[7][3] = new Queen({ 3, 7 }, PlayerTeam::Black, res);

	grid[0][4] = new King({ 4, 0 }, PlayerTeam::White, res);
	grid[7][4] = new King({ 4, 7 }, PlayerTeam::Black, res);

	grid[0][5] = new Bishop({ 5, 0 }, PlayerTeam::White, res);
	grid[7][5] = new Bishop({ 5, 7 }, PlayerTeam::Black, res);

	grid[0][6] = new Knight({ 6, 0 }, PlayerTeam::White, res);
	grid[7][6] = new Knight({ 6, 7 }, PlayerTeam::Black, res);

	grid[0][7] = new Rook({ 7, 0 }, PlayerTeam::White, res);
	grid[7][7] = new Rook({ 7, 7 }, PlayerTeam::Black, res);

	res->InitGrid(grid);
	return res;
}