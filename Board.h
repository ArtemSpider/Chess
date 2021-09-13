#pragma once

#include <map>
#include <cassert>

#include "Coords.h"
#include "Other.h"
#include "PieceMove.h"
#include "Piece.h"


class ChessBoard
{
	vector<vector<Piece*> > grid;

	vector<PieceMove> moves;

	PlayerTeam curTurn;
	
	vector<vector<bool> > visibleByWhite;
	vector<vector<bool> > visibleByBlack;


	struct TestMoveData
	{
		Position from;
		Position to;

		Piece* removedPiece;

		TestMoveData()
			: from(), to(), removedPiece(nullptr)
		{}
		TestMoveData(Position from, Position to, Piece* removedPiece)
			: from(from), to(to), removedPiece(removedPiece)
		{}
	};
	TestMoveData TestMove(Position from, Position to)
	{
		Piece* p = grid[from.y][from.x];

		p->Move(to);

		grid[from.y][from.x] = nullptr;

		TestMoveData data(from, to, grid[to.y][to.x]);
		grid[to.y][to.x] = p;

		UpdatePieces();

		return data;
	}
	void ReverseTestMove(TestMoveData data)
	{
		grid[data.from.y][data.from.x] = grid[data.to.y][data.to.x];
		grid[data.to.y][data.to.x] = data.removedPiece;

		grid[data.from.y][data.from.x]->Move(data.from);

		UpdatePieces();
	}


	void UpdatePieces()
	{
		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
			{
				visibleByWhite[i][j] = false;
				visibleByBlack[i][j] = false;
			}

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (grid[i][j] != nullptr)
				{
					grid[i][j]->Update();

					auto& visible = (grid[i][j]->GetTeam() == PlayerTeam::White ? visibleByWhite : visibleByBlack);
					auto visibleByPiece = grid[i][j]->GetVisible();

					for (auto& p : visibleByPiece)
						visible[p.y][p.x] = true;
				}
	}

	map<Piece*, vector<Position> > legalMoves;
	void UpdateLegalMoves()
	{
		legalMoves.clear();

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (!IsEmpty({ j, i }) && grid[i][j]->GetTeam() == curTurn)
					legalMoves[grid[i][j]] = {};

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
			{
				Piece* p = grid[i][j];
				if (p != nullptr && p->GetTeam() == curTurn)
				{
					auto moves = p->GetMoves();

					for (auto& m : moves)
					{
						auto r = TestMove(p->GetPosition(), m);

						if (!IsCheck())
							legalMoves[p].push_back(m);

						ReverseTestMove(r);
					}
				}
			}

		// en passant
		if (!moves.empty())
		{
			auto lastMove = moves.back();
			if (lastMove.piece == PieceType::Pawn && 
				lastMove.type == PieceMove::MoveType::Move &&
				abs(lastMove.to.y - lastMove.from.y) == 2)
			{
				Position pos = lastMove.to;
				pos.x -= 1;

				if (InBounds(pos))
				{
					const Piece* p = GetPieceAt(pos);
					if (p != nullptr && p->GetType() == PieceType::Pawn && p->GetTeam() == curTurn)
					{
						Position newPos(lastMove.from.x, (lastMove.from.y + lastMove.to.y) / 2);

						auto r = TestMove(p->GetPosition(), newPos);

						if (!IsCheck())
							legalMoves[(Piece*)p].push_back(newPos);

						ReverseTestMove(r);
					}
				}


				pos = lastMove.to;
				pos.x += 1;

				if (InBounds(pos))
				{
					const Piece* p = GetPieceAt(pos);
					if (p != nullptr && p->GetType() == PieceType::Pawn && p->GetTeam() == curTurn)
					{
						Position newPos(lastMove.from.x, (lastMove.from.y + lastMove.to.y) / 2);

						auto r = TestMove(p->GetPosition(), newPos);

						if (!IsCheck())
							legalMoves[(Piece*)p].push_back(newPos);

						ReverseTestMove(r);
					}
				}
			}
		}
	}
public:
	const Size SIZE;

	ChessBoard(Size size) 
		: SIZE(size), 
		grid(size.y, vector<Piece*>(size.x, nullptr)),
		visibleByWhite(size.y, vector<bool>(size.x, false)),
		visibleByBlack(size.y, vector<bool>(size.x, false)),
		moves(), curTurn(PlayerTeam::White) {}
	
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

		Update();
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

	void Update()
	{
		UpdatePieces();
		UpdateLegalMoves();
		/*
		cerr << "By white: " << endl;
		for (int i = 0; i < SIZE.y; i++)
		{
			for (int j = 0; j < SIZE.x; j++)
				cerr << visibleByWhite[SIZE.y - 1 - i][j];
			cerr << endl;
		}
		cerr << endl;

		cerr << "By black: " << endl;
		for (int i = 0; i < SIZE.y; i++)
		{
			for (int j = 0; j < SIZE.x; j++)
				cerr << visibleByBlack[SIZE.y - 1 - i][j];
			cerr << endl;
		}

		cerr << endl;
		cerr << endl;*/
	}

	void MovePiece(Position from, Position to)
	{
		Piece* p = grid[from.y][from.x];

		p->Move(to);

		grid[from.y][from.x] = nullptr;

		bool capture = false;

		// en passant
		if (p->GetType() == PieceType::Pawn && from.x != to.x && grid[to.y][to.x] == nullptr)
		{
			assert(grid[from.y][to.x] != nullptr);

			capture = true;
			delete grid[from.y][to.x];
			grid[from.y][to.x] = nullptr;
		}

		if (grid[to.y][to.x] != nullptr)
		{
			capture = true;
			delete grid[to.y][to.x];
			grid[to.y][to.x] = nullptr;
		}
		grid[to.y][to.x] = p;

		curTurn = (curTurn == PlayerTeam::White ? PlayerTeam::Black : PlayerTeam::White);
		UpdatePieces();


		moves.emplace_back();

		moves.back().from = from;
		moves.back().to = to;
		moves.back().piece = p->GetType();

		moves.back().capture = capture;

		UpdateLegalMoves();

		moves.back().check = IsCheck();
		if (moves.back().check)
			moves.back().mate = IsMate();

		if (moves.back().check)
			cerr << "Check" << endl;
		if (moves.back().mate)
			cerr << "Mate" << endl;
	}

	bool IsCheck() const
	{
		Piece* king = nullptr;
		for (int i = 0; i < SIZE.y && king == nullptr; i++)
			for (int j = 0; j < SIZE.x && king == nullptr; j++)
				if (!IsEmpty({ j, i }) && grid[i][j]->GetTeam() == curTurn && grid[i][j]->GetType() == PieceType::King)
					king = grid[i][j];

		if (king == nullptr)
			throw "King not found";

		return GetVisibleBy(OtherTeam(curTurn), king->GetPosition());
	}
	bool IsMate() const
	{
		for (auto& p : legalMoves)
			if (!p.second.empty())
				return false;
		return true;
	}

	const map<Piece*, vector<Position> >& GetLegalMoves() const
	{
		return legalMoves;
	}
	const vector<Position>& GetLegalMoves(Piece* p) const
	{
		return legalMoves.at(p);
	}
	const vector<Position>& GetLegalMoves(const Piece* p) const
	{
		return legalMoves.at((Piece*)p);
	}


	PlayerTeam GetTurn() const
	{
		return curTurn;
	}


	const vector<vector<bool> >& GetVisibleBy(PlayerTeam team) const
	{
		return (team == PlayerTeam::White ? visibleByWhite : visibleByBlack);
	}
	const vector<vector<bool> >& GetVisibleByWhite() const
	{
		return visibleByWhite;
	}
	const vector<vector<bool> >& GetVisibleByBlack() const
	{
		return visibleByBlack;
	}

	bool GetVisibleBy(PlayerTeam team, Position pos) const
	{
		return (team == PlayerTeam::White ? visibleByWhite : visibleByBlack)[pos.y][pos.x];
	}
	bool GetVisibleByWhite(Position pos) const
	{
		return visibleByWhite[pos.y][pos.x];
	}
	bool GetVisibleByBlack(Position pos) const
	{
		return visibleByBlack[pos.y][pos.x];
	}

	const vector<PieceMove>& GetMovesRecord() const
	{
		return moves;
	}
	const PieceMove GetLastMove() const
	{
		return moves.back();
	}
};