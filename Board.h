#pragma once

#include "Coords.h"
#include "Other.h"
#include "PieceMove.h"
#include "Piece.h"


class ChessBoard
{
	vector<vector<Piece*> > grid;

	vector<PieceMove> moves;

	PlayerTeam curTurn;
public:
	const Size SIZE;

	ChessBoard(Size size) : SIZE(size), grid(size.y, vector<Piece*>(size.x, nullptr)), moves(), curTurn(PlayerTeam::White) {}

	void InitGrid(vector<vector<Piece*> > grid)
	{
		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (this->grid[i][j] != nullptr)
					delete this->grid[i][j];
		this->grid = grid;
	}

	bool InBounds(Position pos) const
	{
		return (pos.x >= 0 && pos.x < SIZE.x) && (pos.y >= 0 && pos.y < SIZE.y);
	}
	bool IsEmpty(Position pos) const
	{
		return grid[pos.y][pos.x] == nullptr;
	}
	PlayerTeam GetTeam(Position pos) const
	{
		if (IsEmpty(pos))
			throw "There is no piece";
		return grid[pos.y][pos.x]->GetTeam();
	}
	const Piece* GetPieceAt(Position pos) const
	{
		return grid[pos.y][pos.x];
	}
	bool IsPossibleMove(Piece* piece, Position to) const
	{
		if (piece == nullptr) return false;
		if (!InBounds(to)) return false;

		Piece* toPiece = grid[to.y][to.x];

		if (toPiece != nullptr && piece->GetTeam() == toPiece->GetTeam())
			return false;

		return true;
	}

	void MovePiece(Position from, Position to)
	{
		if (IsEmpty(from)) throw "There is no piece";
		Piece* p = grid[from.y][from.x];

		if (p->GetTeam() != curTurn) throw "Wrong turn";
		if (!IsPossibleMove(p, to)) throw "Impossible move";

		auto possibleMoves = p->GetMoves();
		if (find(possibleMoves.begin(), possibleMoves.end(), to) == possibleMoves.end())
			throw "Impossible move";

		p->Move(to);

		PieceMove move;

		move.from = from;
		move.to = to;
		move.piece = p->GetType();

		if (!IsEmpty(to))
			move.capture = true;

		move.check = IsCheck(OtherTeam(p->GetTeam()));
		if (move.check)
			move.mate = IsMate(OtherTeam(p->GetTeam()));

		moves.push_back(move);
	}

	bool IsCheck(PlayerTeam team) const
	{
		Piece* king = nullptr;
		for (int i = 0; i < 8 && king == nullptr; i++)
			for (int j = 0; j < 8 && king == nullptr; j++)
				if (!IsEmpty({ i, j }) && grid[i][j]->GetTeam() == team && grid[i][j]->GetType() == PieceType::King)
					king = grid[i][j];

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				if (!IsEmpty({ i, j }) && grid[i][j]->GetTeam() != team)
				{
					auto possibleMoves = grid[i][j]->GetMoves();
					if (find(possibleMoves.begin(), possibleMoves.end(), king->GetPosition()) != possibleMoves.end())
						return true;
				}
		return false;
	}
	bool IsMate(PlayerTeam team) const // TODO
	{
		if (!IsCheck(team)) return false;
		return false;
	}


	PlayerTeam GetTurn() const
	{
		return curTurn;
	}
};