#pragma once

#include <map>
#include <cassert>

#include "Coords.h"
#include "Other.h"
#include "PieceMove.h"
#include "Piece.h"



struct GameState
{
	enum class State
	{
		Game = -1, // Game is still going
		WhiteWon,
		BlackWon,
		Draw
	};

	State state;
	string reason;

	GameState() : state(State::Game), reason() {}
	GameState(State state, string reason) : state(state), reason(reason) {}
};

class ChessBoard
{
	vector<vector<Piece*> > grid;

	vector<PieceMove> moves; // record of all moves

	PlayerTeam curTurn;
	
	vector<vector<bool> > visibleByWhite;
	vector<vector<bool> > visibleByBlack;

	GameState state;


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

		p->SetPosition(to);

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

		grid[data.from.y][data.from.x]->SetPosition(data.from);

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
				if (!IsEmpty({ j, i }))
				{
					grid[i][j]->Update();

					auto& visible = (grid[i][j]->GetTeam() == PlayerTeam::White ? visibleByWhite : visibleByBlack);
					const vector<Position>& visibleByPiece = grid[i][j]->GetVisible();

					for (const Position& p : visibleByPiece)
						visible[p.y][p.x] = true;
				}
	}

	map<Piece*, vector<Position> > legalMoves;
	void AddEnPassant()
	{
		if (!moves.empty())
		{
			PieceMove lastMove = GetLastMove();
			if (lastMove.piece == PieceType::Pawn &&
				lastMove.type == PieceMove::MoveType::Move &&
				abs(lastMove.to.y - lastMove.from.y) == 2)
			{
				auto CheckAndPush = [&](Position pos)
				{
					if (InBounds(pos) && !IsEmpty(pos) && GetType(pos) == PieceType::Pawn && GetTeam(pos) == curTurn)
					{
						Piece* p = _GetPieceAt(pos);

						Position newPos(lastMove.from.x, (lastMove.from.y + lastMove.to.y) / 2);

						auto r = TestMove(p->GetPosition(), newPos);

						if (!IsCheck())
							legalMoves[p].push_back(newPos);

						ReverseTestMove(r);
					}
				};

				CheckAndPush(lastMove.to + Position(1, 0));
				CheckAndPush(lastMove.to - Position(1, 0));
			}
		}
	}
	void AddCastles()
	{
		Piece* king = nullptr;
		Piece* leftRook = nullptr;
		Piece* rightRook = nullptr;

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (!IsEmpty({ j, i }) && GetTeam({ j, i }) == curTurn)
				{
					if (GetType({ j, i }) == PieceType::King)
						king = grid[i][j];

					if (GetType({ j, i }) == PieceType::Rook)
						(leftRook == nullptr ? leftRook : rightRook) = grid[i][j];
				}

		if (king == nullptr)
			throw "King not found";

		if (king->HasMoved())
			return;

		if (leftRook == nullptr && rightRook == nullptr)
			return;

		if (leftRook->GetPosition().x > king->GetPosition().x)
			swap(leftRook, rightRook);

		if (leftRook != nullptr && !leftRook->HasMoved()) // long castle
		{
			Position pos = king->GetPosition();
			pos.x--;

			bool canCastle = true;
			for (; canCastle; pos.x--)
			{
				if (GetPieceAt(pos) == leftRook)
					break;

				if (!IsEmpty(pos))
				{
					canCastle = false;
					break;
				}
			}

			if (canCastle)
			{
				auto r1 = TestMove(king->GetPosition(), Position(king->GetPosition().x - 2, king->GetPosition().y));
				auto r2 = TestMove(leftRook->GetPosition(), Position(king->GetPosition().x + 1, king->GetPosition().y));

				if (!IsCheck())
					legalMoves[king].push_back(king->GetPosition());

				ReverseTestMove(r2);
				ReverseTestMove(r1);
			}
		}
		if (rightRook != nullptr && !rightRook->HasMoved()) // short castle
		{
			Position pos = king->GetPosition();
			pos.x++;

			bool canCastle = true;
			for (; canCastle; pos.x++)
			{
				if (GetPieceAt(pos) == rightRook)
					break;

				if (!IsEmpty(pos))
				{
					canCastle = false;
					break;
				}
			}

			if (canCastle)
			{
				auto r1 = TestMove(king->GetPosition(), Position(king->GetPosition().x + 2, king->GetPosition().y));
				auto r2 = TestMove(rightRook->GetPosition(), Position(king->GetPosition().x - 1, king->GetPosition().y));

				if (!IsCheck())
					legalMoves[king].push_back(king->GetPosition());

				ReverseTestMove(r2);
				ReverseTestMove(r1);
			}
		}
	}

	void UpdateLegalMoves()
	{
		legalMoves.clear();

		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (!IsEmpty({ j, i }) && GetTeam({ j, i }) == curTurn)
				{
					legalMoves[grid[i][j]] = {};

					Piece* p = _GetPieceAt({ j, i });
					auto moves = p->GetMoves(); // Must copy because of IsCheck
					for (const auto& m : moves)
					{
						auto r = TestMove(p->GetPosition(), m);

						if (!IsCheck())
							legalMoves[p].push_back(m);

						ReverseTestMove(r);
					}
				}

		AddEnPassant();
		AddCastles();
	}


	GameState CheckDraw() const
	{
		// Егорка сделай пж

		// ф-ция возвращает GameState() если не ничья и игра продолжается
		// иначе GameState(GameState::State::Draw, "какая-то причина")

		return GameState();
	}

	void UpdateState()
	{
		state = GameState();

		if (IsMate())
		{
			state.state = (curTurn == PlayerTeam::White ? GameState::State::BlackWon : GameState::State::WhiteWon);
			state.reason = "checkmate";

			return;
		}
		else state = CheckDraw();
	}

	void ClearGrid()
	{
		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (this->grid[i][j] != nullptr)
				{
					delete this->grid[i][j];
					this->grid[i][j] = nullptr;
				}
	}

	Piece* _GetPieceAt(Position pos)
	{
		return grid[pos.y][pos.x];
	}
public:
	const Size SIZE;

	ChessBoard(Size size) :
		SIZE(size), 
		grid(size.y, vector<Piece*>(size.x, nullptr)),
		visibleByWhite(size.y, vector<bool>(size.x, false)),
		visibleByBlack(size.y, vector<bool>(size.x, false)),
		moves(), curTurn(PlayerTeam::White) {}
	
	~ChessBoard()
	{
		ClearGrid();
	}

	void InitGrid(vector<vector<Piece*> > grid)
	{
		ClearGrid();
		this->grid = grid;

		Update();
	}

	bool InBounds(Position pos) const
	{
		return (pos.x >= 0 && pos.x < SIZE.x) && (pos.y >= 0 && pos.y < SIZE.y);
	}


	const Piece* GetPieceAt(Position pos) const
	{
		return grid[pos.y][pos.x];
	}

	bool IsEmpty(Position pos) const
	{
		return GetPieceAt(pos) == nullptr;
	}
	PlayerTeam GetTeam(Position pos) const
	{
		if (IsEmpty(pos))
			throw "There is no piece";
		return GetPieceAt(pos)->GetTeam();
	}
	PieceType GetType(Position pos) const
	{
		if (IsEmpty(pos))
			throw "There is no piece";
		return GetPieceAt(pos)->GetType();
	}
	bool HasMoved(Position pos) const
	{
		if (IsEmpty(pos))
			throw "There is no piece";
		return GetPieceAt(pos)->HasMoved();
	}

	void Update()
	{
		UpdatePieces();
		UpdateLegalMoves();
		UpdateState();
	}

	void MovePiece(Position from, Position to)
	{
		Piece* p = _GetPieceAt(from);

		moves.emplace_back();

		moves.back().movedBefore = p->HasMoved();
		moves.back().from = from;
		moves.back().to = to;
		moves.back().piece = p->GetType();

		p->Move(to);

		grid[from.y][from.x] = nullptr;

		bool capture = false;

		// en passant
		if (p->GetType() == PieceType::Pawn && from.x != to.x && IsEmpty(to))
		{
			capture = true;
			delete grid[from.y][to.x];
			grid[from.y][to.x] = nullptr;
		}

		// castles
		if (p->GetType() == PieceType::King && abs(from.x - to.x) == 2)
		{
			Piece* rook = nullptr;

			int dir = (to.x - from.x) / 2; // direction of castling
			
			for (Position pos = p->GetPosition(); rook == nullptr; pos.x += dir)
				if (!IsEmpty(pos) && GetTeam(pos) == curTurn && GetType(pos) == PieceType::Rook)
					rook = grid[pos.y][pos.x];

			grid[rook->GetPosition().y][rook->GetPosition().x] = nullptr;
			rook->Move(Position(p->GetPosition().x - dir, p->GetPosition().y));
			grid[rook->GetPosition().y][rook->GetPosition().x] = rook;
		}

		if (!IsEmpty(to))
		{
			capture = true;
			delete grid[to.y][to.x];
			grid[to.y][to.x] = nullptr;
		}
		grid[to.y][to.x] = p;

		curTurn = (curTurn == PlayerTeam::White ? PlayerTeam::Black : PlayerTeam::White);
		UpdatePieces();

		moves.back().capture = capture;

		UpdateLegalMoves();

		moves.back().check = IsCheck();
		if (moves.back().check)
			moves.back().mate = IsMate();

		UpdateState();
	}

	bool IsCheck() const
	{
		Piece* king = nullptr;
		for (int i = 0; i < SIZE.y && king == nullptr; i++)
			for (int j = 0; j < SIZE.x && king == nullptr; j++)
				if (!IsEmpty({ j, i }) && GetTeam({ j, i }) == curTurn && GetType({j, i}) == PieceType::King)
					king = grid[i][j];

		if (king == nullptr)
			throw "King not found";

		return GetVisibleBy(OtherTeam(curTurn), king->GetPosition());
	}
	bool IsMate() const
	{
		if (!IsCheck())
			return false;

		for (auto& p : legalMoves)
			if (!p.second.empty())
				return false;
		return true;
	}
	bool IsStalemate() const
	{
		if (IsCheck())
			return false;

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

	const GameState GetGameState() const
	{
		return state;
	}
};