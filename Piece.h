#pragma once

#include <vector>
#include <string>

#include "Coords.h"
#include "Other.h"

using namespace std;


class ChessBoard;

class Piece
{
protected:
	Position pos;

	PlayerTeam team;

	vector<Position> possibleMoves;
	vector<Position> visible;

	bool moved;
public:
	const ChessBoard* board;

	Piece(Position pos, PlayerTeam team, const ChessBoard* board) : pos(pos), team(team), moved(false), board(board) {}

	virtual ~Piece() {}


	virtual void Update() = 0; // Updates possibleMoves and visible


	virtual vector<Position> GetMoves() const
	{
		return possibleMoves;
	}
	virtual vector<Position> GetVisible() const
	{
		return visible;
	}

	virtual string GetName() const = 0;
	virtual PieceType GetType() const = 0;

	virtual void Move(Position newPos)
	{
		pos = newPos;
		moved = true;
	}
	virtual void TestMove(Position newPos)
	{
		pos = newPos;
	}

	Position GetPosition() const
	{
		return pos;
	}
	PlayerTeam GetTeam() const
	{
		return team;
	}

	bool HasMoved() const
	{
		return moved;
	}
};