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
public:
	const ChessBoard* board;

	Piece(Position pos, PlayerTeam team, const ChessBoard* board) : pos(pos), team(team), board(board) {}

	virtual ~Piece() {}


	virtual void Update() = 0; // Updates possibleMoves and visible


	virtual vector<Position> GetMoves() const = 0
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
		auto possibleMoves = GetMoves();
		if (find(possibleMoves.begin(), possibleMoves.end(), newPos) != possibleMoves.end())
			pos = newPos;
		throw "Move is not possible";
	}

	Position GetPosition() const
	{
		return pos;
	}
	PlayerTeam GetTeam() const
	{
		return team;
	}
};