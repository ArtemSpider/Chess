#pragma once

#include <map>
#include <string>
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

struct TimeControl
{
	int time;
	int increment;
};

class ChessBoard
{
	vector<vector<Piece*> > grid;

	int turnsWithoutCapture;			// for 50-move rule
	map<string, int> prevBoards;		// for 3-fold repetition
	
	int curMoveInd;						// index of a move on a grid, -1 on the first move
	vector<PieceMove> moves;			// record of all moves

	PlayerTeam curTurn;					// turn on the board
	PlayerTeam realTurn;				// turn not considering moveBackwards
	
	vector<vector<bool> > visibleByWhite;
	vector<vector<bool> > visibleByBlack;

	GameState state;


	bool withoutTime;
	const TimeControl timeControl;
	int remainingTimeWhite;	// in seconds
	int remainingTimeBlack;	// in seconds


	struct TestMoveData
	{
		Position from;
		Position to;

		Piece* removedPiece;

		TestMoveData() :
			from(), to(), removedPiece(nullptr)
		{}
		TestMoveData(Position from, Position to, Piece* removedPiece) :
			from(from), to(to), removedPiece(removedPiece)
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
			if (lastMove.piece->GetType() == PieceType::Pawn &&
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
		if (IsCheck()) return;

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
		auto curHash = prevBoards.find(GetHash())->second;
		if (curHash == 3)
			return GameState(GameState::State::Draw, "by repetition");
		else if (IsStalemate())
			return GameState(GameState::State::Draw, "by stalemate");
		else if (turnsWithoutCapture == 50)
			return GameState(GameState::State::Draw, "by 50-move rule");
		return GameState();
	}

	void CheckTimeout()
	{
		if (state.state != GameState::State::Game) return;

		if (!withoutTime && GetRemainingTime() <= 0)
		{
			state.state = (curTurn == PlayerTeam::White ? GameState::State::BlackWon : GameState::State::WhiteWon);
			state.reason = "by timeout";
		}
	}
	void UpdateState()
	{
		if (state.state != GameState::State::Game) return;

		state = GameState();

		if (IsMate())
		{
			state.state = (curTurn == PlayerTeam::White ? GameState::State::BlackWon : GameState::State::WhiteWon);
			state.reason = "by checkmate";
		}
		else state = CheckDraw();
	}

	void Clean()
	{
		for (int i = 0; i < SIZE.y; i++)
			for (int j = 0; j < SIZE.x; j++)
				if (this->grid[i][j] != nullptr)
				{
					delete this->grid[i][j];
					this->grid[i][j] = nullptr;
				}

		for (PieceMove m : moves)
		{
			if (m.captured != nullptr)
			{
				delete m.captured;
				m.captured = nullptr;
			}
		}

		prevBoards.clear();
		moves.clear();
		state = GameState();
	}

	Piece* _GetPieceAt(Position pos)
	{
		return grid[pos.y][pos.x];
	}

	string GetNotation(PieceMove move)
	{
		string res;
		if (move.type == PieceMove::MoveType::CastleShort)
			res = "O-O";
		else if (move.type == PieceMove::MoveType::CastleLong)
			res = "O-O-O";
		else if (move.piece->GetType() == PieceType::Pawn)
		{
			res = ToNotation(move.to);
			if (move.captured != nullptr)
				res = FileToNotation(move.from) + 'x' + res;

			if (move.type == PieceMove::MoveType::PromotionKnight)
				res = res + "=N";
			if (move.type == PieceMove::MoveType::PromotionBishop)
				res = res + "=B";
			if (move.type == PieceMove::MoveType::PromotionRook)
				res = res + "=R";
			if (move.type == PieceMove::MoveType::PromotionQueen)
				res = res + "=Q";
		}
		else
		{
			switch (move.piece->GetType())
			{
			case PieceType::Knight: res += 'N'; break;
			case PieceType::Bishop: res += 'B'; break;
			case PieceType::Rook:   res += 'R'; break;
			case PieceType::Queen:  res += 'Q'; break;
			case PieceType::King:   res += 'K'; break;
			}

			bool ambiguity = false;
			bool sameFiles = false;
			bool sameRanks = false;

			for (int i = 0; i < grid.size(); i++)
				for (int j = 0; j < grid[i].size(); j++)
					if (grid[i][j] != nullptr && grid[i][j] != move.piece && 
						grid[i][j]->GetTeam() == move.piece->GetTeam() && 
						grid[i][j]->GetType() == move.piece->GetType())
					{
						const auto& possibleMoves = GetLegalMoves(grid[i][j]);
						if (find(possibleMoves.begin(), possibleMoves.end(), move.to) != possibleMoves.end())
						{
							ambiguity = true;
							if (i == move.from.y)
								sameRanks = true;
							if (j == move.from.x)
								sameFiles = true;
						}
					}
			if (ambiguity)
			{
				if (sameFiles)
				{
					if (sameRanks)
						res += ToNotation(move.from);
					else
						res += RankToNotation(move.from);
				}
				else
					res += FileToNotation(move.from);
			}
			if (move.captured != nullptr)
				res += 'x';
			res += ToNotation(move.to);
		}

		return res;
	}
public:
	const Size SIZE;

	ChessBoard(Size size, TimeControl timeControl) :
		SIZE(size), curMoveInd(-1),
		grid(size.y, vector<Piece*>(size.x, nullptr)),
		visibleByWhite(size.y, vector<bool>(size.x, false)),
		visibleByBlack(size.y, vector<bool>(size.x, false)),
		moves(), curTurn(PlayerTeam::White), realTurn(PlayerTeam::White),
		promoteToWhite(PieceType::Queen), promoteToBlack(PieceType::Queen),
		withoutTime(false), timeControl(timeControl),
		remainingTimeWhite(timeControl.time), remainingTimeBlack(timeControl.time),
		turnsWithoutCapture(0)
	{}
	
	~ChessBoard()
	{
		Clean();
	}

	void InitGrid(vector<vector<Piece*> > grid)
	{
		Clean();
		this->grid = grid;
		prevBoards[GetHash()] = 1;

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

	void MoveBackward()
	{
		if (curMoveInd == -1)
			return;

		PieceMove move = moves[curMoveInd--];

		Position from = move.from;
		Position to = move.to;

		Piece* captured = move.captured;
		Piece* p = move.piece;

		if (move.type == PieceMove::MoveType::Move)
		{
			p->Move(from);
			grid[from.y][from.x] = p;
			grid[to.y][to.x] = nullptr;

			if (captured != nullptr)
				grid[captured->GetPosition().y][captured->GetPosition().x] = captured;

			p->moved = move.movedBefore;
		}
		else if (move.type == PieceMove::MoveType::CastleShort)
		{
			// move the king back
			p->Move(from);
			grid[from.y][from.x] = p;
			grid[to.y][to.x] = nullptr;

			p->moved = false;

			// move the rook back
			Piece* rook = grid[to.y][to.x - 1];

			Position prevPos = to + Position(1, 0);
			rook->Move(prevPos);
			grid[prevPos.y][prevPos.x] = rook;
			grid[to.y][to.x - 1] = nullptr;

			rook->moved = false;
		}
		else if (move.type == PieceMove::MoveType::CastleLong)
		{
			// move the king back
			p->Move(from);
			grid[from.y][from.x] = p;
			grid[to.y][to.x] = nullptr;

			p->moved = false;

			// move the rook back
			Piece* rook = grid[to.y][to.x + 1];

			Position prevPos = to - Position(2, 0);
			rook->Move(prevPos);
			grid[prevPos.y][prevPos.x] = rook;
			grid[to.y][to.x + 1] = nullptr;

			rook->moved = false;
		}
		else if (move.type == PieceMove::MoveType::PromotionKnight ||
			move.type == PieceMove::MoveType::PromotionBishop ||
			move.type == PieceMove::MoveType::PromotionRook ||
			move.type == PieceMove::MoveType::PromotionQueen)
		{
			grid[to.y][to.x] = captured;
			grid[from.y][from.x] = p;
		}

		curTurn = OtherTeam(curTurn);

		Update();
	}
	void MoveForward()
	{
		if (curMoveInd + 1 == moves.size())
			return;

		PieceMove move = moves[++curMoveInd];

		Position from = move.from;
		Position to = move.to;

		Piece* captured = move.captured;
		Piece* p = move.piece;

		if (move.type == PieceMove::MoveType::Move)
		{
			if (captured != nullptr)
				grid[captured->GetPosition().y][captured->GetPosition().x] = nullptr;

			p->Move(to);
			grid[to.y][to.x] = p;
			grid[from.y][from.x] = nullptr;
		}
		else if (move.type == PieceMove::MoveType::CastleShort)
		{
			// move the king forward
			p->Move(to);
			grid[to.y][to.x] = p;
			grid[from.y][from.x] = nullptr;

			// move the rook forward
			Piece* rook = grid[to.y][to.x + 1];

			Position newPos = to - Position(1, 0);
			rook->Move(newPos);
			grid[newPos.y][newPos.x] = rook;
			grid[to.y][to.x + 1] = nullptr;
		}
		else if (move.type == PieceMove::MoveType::CastleLong)
		{
			// move the king forward
			p->Move(to);
			grid[to.y][to.x] = p;
			grid[from.y][from.x] = nullptr;

			// move the rook forward
			Piece* rook = grid[to.y][to.x - 2];

			Position newPos = to + Position(1, 0);
			rook->Move(newPos);
			grid[newPos.y][newPos.x] = rook;
			grid[to.y][to.x - 2] = nullptr;
		}
		else if (move.type == PieceMove::MoveType::PromotionKnight || 
				 move.type == PieceMove::MoveType::PromotionBishop ||
				 move.type == PieceMove::MoveType::PromotionRook ||
				 move.type == PieceMove::MoveType::PromotionQueen)
		{
			grid[to.y][to.x] = move.promoted;
			grid[from.y][from.x] = nullptr;
		}

		curTurn = OtherTeam(curTurn);

		Update();
	}

	PieceType promoteToWhite;	// what type white pawn promotes to
	PieceType promoteToBlack;	// what type black pawn promotes to
	void MovePiece(Position from, Position to)
	{
		if (!withoutTime)
			ChangeRemainingTime(timeControl.increment);

		Piece* p = _GetPieceAt(from);

		if (curMoveInd + 1 < moves.size())
			return;

		moves.emplace_back();

		curMoveInd = moves.size() - 1;

		moves.back().movedBefore = p->HasMoved();
		moves.back().from = from;
		moves.back().to = to;
		moves.back().piece = p;
		moves.back().type = PieceMove::MoveType::Move;

		p->Move(to);

		grid[from.y][from.x] = nullptr;

		Piece* capture = nullptr;

		// en passant
		if (p->GetType() == PieceType::Pawn && from.x != to.x && IsEmpty(to))
		{
			capture = grid[from.y][to.x];
			grid[from.y][to.x] = nullptr;
		}
		else
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

			moves.back().type = (dir == -1 ? PieceMove::MoveType::CastleLong : PieceMove::MoveType::CastleShort);
		}
		else
		// capture
		if (!IsEmpty(to))
		{
			capture = grid[to.y][to.x];
			grid[to.y][to.x] = nullptr;
		}
		grid[to.y][to.x] = p;
		moves.back().captured = capture;

		// promotion
		if (p->GetType() == PieceType::Pawn && (to.y == 0 || to.y == 7))
		{
			PieceType promoteTo = (curTurn == PlayerTeam::White ? promoteToWhite : promoteToBlack);

			p->SetPosition(from);
			grid[to.y][to.x] = moves.back().promoted = MakePiece(promoteTo, to, p->GetTeam(), this);

			switch (promoteTo)
			{
			case PieceType::Knight: moves.back().type = PieceMove::MoveType::PromotionKnight; break;
			case PieceType::Bishop: moves.back().type = PieceMove::MoveType::PromotionBishop; break;
			case PieceType::Rook:   moves.back().type = PieceMove::MoveType::PromotionRook;   break;
			case PieceType::Queen:  moves.back().type = PieceMove::MoveType::PromotionQueen;  break;
			}
		}
		moves.back().notation = GetNotation(moves.back());

		curTurn = (curTurn == PlayerTeam::White ? PlayerTeam::Black : PlayerTeam::White);
		realTurn = curTurn;
		UpdatePieces();

		UpdateLegalMoves();

		moves.back().check = IsCheck();
		if (moves.back().check)
			moves.back().mate = IsMate();

		if (moves.back().mate)
			moves.back().notation += '#';
		else if (moves.back().check)
			moves.back().notation += '+';

		turnsWithoutCapture++;
		if (capture != nullptr || p->GetType() == PieceType::Pawn)
			turnsWithoutCapture = 0;

		prevBoards[GetHash()]++;

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

	bool IsLastMove() const
	{
		return curMoveInd + 1 == moves.size();
	}
	void ToLastMove()
	{
		while (!IsLastMove()) MoveForward();
	}

	bool WithoutTime() const
	{
		return withoutTime;
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
		if (moves.empty())
			throw "There are no moves";
		return moves.back();
	}

	GameState GetGameState() const
	{
		return state;
	}

	TimeControl GetTimeControl() const
	{
		return timeControl;
	}

	int GetRemainingTimeFor(PlayerTeam team) const
	{
		return (team == PlayerTeam::White ? remainingTimeWhite : remainingTimeBlack);
	}
	int GetRemainingTime() const
	{
		return GetRemainingTimeFor(realTurn);
	}

	void SetRemainingTimeFor(PlayerTeam team, int time)
	{
		(team == PlayerTeam::White ? remainingTimeWhite : remainingTimeBlack) = time;

		if (withoutTime)
		{
			remainingTimeWhite = timeControl.time;
			remainingTimeBlack = timeControl.time;
		}
		CheckTimeout();
	}
	void SetRemainingTime(int time)
	{
		SetRemainingTimeFor(realTurn, time);
	}

	void ChangeRemainingTime(int delta)
	{
		SetRemainingTime(max(0, GetRemainingTime() + delta));

		if (withoutTime)
		{
			remainingTimeWhite = timeControl.time;
			remainingTimeBlack = timeControl.time;
		}
		CheckTimeout();
	}

	/*
	* Returns string with size of 32, unique for every board
	* First 4 bits of i sumbol is grid[i / 8][i % 8], second 4 is grid[i / 8][i % 8 + 1]
	*/
	string GetHash() const
	{
		string hash;
		for (int i = 0; i < 64; i += 2)
		{
			int x = i / 8, y = i % 8;
			unsigned char c = 0;
			if (grid[x][y + 1] != nullptr)
				c |= ((int)grid[x][y + 1]->GetType() + 1) | (8 * (grid[x][y + 1]->GetTeam() == PlayerTeam::Black));
			c <<= 4;
			if (grid[x][y] != nullptr)
				c |= ((int)grid[x][y]->GetType() + 1) | (8 * (grid[x][y]->GetTeam() == PlayerTeam::Black));
			hash += c;
		}
		return hash;
	}

	friend class Game;
};