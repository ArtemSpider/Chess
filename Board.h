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
	
	vector<Position> visibleByWhite;
	vector<Position> visibleByBlack;
public:
	const Size SIZE;

	ChessBoard(Size size) : SIZE(size), grid(size.y, vector<Piece*>(size.x, nullptr)), moves(), curTurn(PlayerTeam::White) {}
	
	~ChessBoard()
	{
		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (this->grid[i][j] != nullptr)
				{
					delete this->grid[i][j];
					this->grid[i][j] = nullptr;
				}
	}

	void InitGrid(vector<vector<Piece*> > grid)
	{
		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (this->grid[i][j] != nullptr)
				{
					delete this->grid[i][j];
					this->grid[i][j] = nullptr;
				}
		this->grid = grid;

		UpdatePieces();
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

	void UpdatePieces()
	{
		visibleByWhite.clear();
		visibleByBlack.clear();

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (grid[i][j] != nullptr)
				{
					grid[i][j]->Update();

					auto visible = (grid[i][j]->GetTeam() == PlayerTeam::White ? visibleByWhite : visibleByBlack);
					auto visibleByPiece = grid[i][j]->GetVisible();
					visible.insert(visible.end(), visibleByPiece.begin(), visibleByPiece.end());
				}
	}
	void MovePiece(Position from, Position to)
	{
		Piece* p = grid[from.y][from.x];

		p->Move(to);

		grid[from.y][from.x] = nullptr;

		if (grid[to.y][to.x] != nullptr)
			delete grid[to.y][to.x];
		grid[to.y][to.x] = p;

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

		curTurn = (curTurn == PlayerTeam::White ? PlayerTeam::Black : PlayerTeam::White);
		UpdatePieces();
	}

	bool IsCheck(PlayerTeam team) const
	{
		Piece* king = nullptr;
		for (int i = 0; i < SIZE.y && king == nullptr; i++)
			for (int j = 0; j < SIZE.x && king == nullptr; j++)
				if (!IsEmpty({ j, i }) && grid[i][j]->GetTeam() == team && grid[i][j]->GetType() == PieceType::King)
					king = grid[i][j];

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (!IsEmpty({ j, i }) && grid[i][j]->GetTeam() != team)
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

	const vector<Position>& GetVisibleBy(PlayerTeam team)
	{
		return (team == PlayerTeam::White ? visibleByWhite : visibleByBlack);
	}
	const vector<Position>& GetVisibleByWhite()
	{
		return visibleByWhite;
	}
	const vector<Position>& GetVisibleByBlack()
	{
		return visibleByBlack;
	}
};