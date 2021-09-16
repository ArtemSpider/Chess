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

	const ChessBoard* board;

	vector<Position> possibleMoves;
	vector<Position> visible;

	bool moved;
public:
	Piece(Position pos, PlayerTeam team, const ChessBoard* board) :
		pos(pos), team(team), board(board),
		possibleMoves(), visible(),
		moved(false)
	{}

	virtual ~Piece() {}


	virtual void Update() = 0; // Updates possibleMoves and visible


	virtual string GetName() const = 0;
	virtual PieceType GetType() const = 0;

	virtual const vector<Position>& GetMoves() const
	{
		return possibleMoves;
	}
	virtual const vector<Position>& GetVisible() const
	{
		return visible;
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


	virtual void Move(Position newPos)
	{
		pos = newPos;
		moved = true;
	}
	virtual void SetPosition(Position newPos)
	{
		pos = newPos;
	}

	friend class ChessBoard;
};


Piece* MakeQueen(Position pos, PlayerTeam team, const ChessBoard* board);
Piece* MakeKnight(Position pos, PlayerTeam team, const ChessBoard* board);
Piece* MakeBishop(Position pos, PlayerTeam team, const ChessBoard* board);
Piece* MakeRook(Position pos, PlayerTeam team, const ChessBoard* board);
Piece* MakeQueen(Position pos, PlayerTeam team, const ChessBoard* board);
Piece* MakeKing(Position pos, PlayerTeam team, const ChessBoard* board);