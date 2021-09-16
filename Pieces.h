#pragma once

#include "Coords.h"
#include "Other.h"
#include "Piece.h"
#include "Board.h"


class Pawn : public Piece
{
public:
	Pawn(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	void Update() override
	{
		possibleMoves.clear();
		visible.clear();

		bool firstMove = false;
		int dir;
		if (GetTeam() == PlayerTeam::White)
		{
			dir = 1;
			firstMove = (pos.y == 1);
		}
		else
		{
			dir = -1;
			firstMove = (pos.y == 6);
		}

		if (firstMove)
		{
			Position newPos = pos + Position(0, 2 * dir);
			if (board->InBounds(newPos) && board->IsEmpty(newPos) && board->IsEmpty(pos + Position(0, dir)))
				possibleMoves.push_back(newPos);
		}

		{
			Position newPos = pos + Position(0, dir);
			if (board->InBounds(newPos) && board->IsEmpty(newPos))
				possibleMoves.push_back(newPos);
		}

		{
			Position newPos = pos + Position(-1, dir);
			if (board->InBounds(newPos))
			{
				visible.push_back(newPos);
				if (!board->IsEmpty(newPos) && board->GetTeam(newPos) != GetTeam())
					possibleMoves.push_back(newPos);
			}

			newPos = pos + Position(+1, dir);
			if (board->InBounds(newPos))
			{
				visible.push_back(newPos);
				if (!board->IsEmpty(newPos) && board->GetTeam(newPos) != GetTeam())
					possibleMoves.push_back(newPos);
			}
		}

		// en passant implemented in ChessBoard
	}

	string GetName() const override
	{
		return "Pawn";
	}
	PieceType GetType() const override
	{
		return PieceType::Pawn;
	}
};

class Knight : public Piece
{
public:
	Knight(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	void Update() override
	{
		possibleMoves.clear();
		visible.clear();

		const Position dpos[] = { {1, -2}, {2, -1}, {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2} };

		for (int d = 0; d < 8; d++)
		{
			Position newPos = pos + dpos[d];
			if (board->InBounds(newPos))
			{
				visible.push_back(newPos);
				if (board->IsEmpty(newPos) || board->GetTeam(newPos) != GetTeam())
					possibleMoves.push_back(newPos);
			}
		}
	}

	string GetName() const override
	{
		return "Knight";
	}
	PieceType GetType() const override
	{
		return PieceType::Knight;
	}
};

class Bishop : public Piece
{
	bool CheckAndPush(Position newPos)
	{
		if (board->InBounds(newPos))
		{
			visible.push_back(newPos);
			if (!board->IsEmpty(newPos))
			{
				if (board->GetTeam(newPos) != GetTeam())
					possibleMoves.push_back(newPos);
				return false;
			}
			else possibleMoves.push_back(newPos);
		}
		else return false;

		return true;
	};
public:
	Bishop(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	void Update() override
	{
		possibleMoves.clear();
		visible.clear();

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(+i, +i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(-i, -i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(-i, +i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(+i, -i))) break;
	}

	string GetName() const override
	{
		return "Bishop";
	}
	PieceType GetType() const override
	{
		return PieceType::Bishop;
	}
};

class Rook : public Piece
{
	bool CheckAndPush(Position newPos)
	{
		if (board->InBounds(newPos))
		{
			visible.push_back(newPos);
			if (!board->IsEmpty(newPos))
			{
				if (board->GetTeam(newPos) != GetTeam())
					possibleMoves.push_back(newPos);
				return false;
			}
			else possibleMoves.push_back(newPos);
		}
		else return false;

		return true;
	};
public:
	Rook(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	void Update() override
	{
		possibleMoves.clear();
		visible.clear();

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(+i, 0))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(-i, 0))) break;

		for (int j = 1;; j++)
			if (!CheckAndPush(pos + Position(0, +j))) break;

		for (int j = 1;; j++)
			if (!CheckAndPush(pos + Position(0, -j))) break;

		// castle implemented in ChessBoard
	}

	string GetName() const override
	{
		return "Rook";
	}
	PieceType GetType() const override
	{
		return PieceType::Rook;
	}
};

class King : public Piece
{
public:
	King(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	void Update() override
	{
		possibleMoves.clear();
		visible.clear();

		const Position dpos[] = { {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1} };

		for (int d = 0; d < 8; d++)
		{
			Position newPos = pos + dpos[d];
			if (board->InBounds(newPos))
			{
				visible.push_back(newPos);
				if (board->IsEmpty(newPos) || board->GetTeam(newPos) != GetTeam())				
					possibleMoves.push_back(newPos);
			}
		}

		// castle implemented in ChessBoard
	}

	string GetName() const override
	{
		return "King";
	}
	PieceType GetType() const override
	{
		return PieceType::King;
	}
};

class Queen : public Piece
{
	bool CheckAndPush(Position newPos)
	{
		if (board->InBounds(newPos))
		{
			visible.push_back(newPos);
			if (!board->IsEmpty(newPos))
			{
				if (board->GetTeam(newPos) != GetTeam())
					possibleMoves.push_back(newPos);
				return false;
			}
			else possibleMoves.push_back(newPos);
		}
		else return false;

		return true;
	};
public:
	Queen(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	void Update() override
	{
		possibleMoves.clear();
		visible.clear();

		// diagonals
		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(+i, +i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(-i, -i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(-i, +i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(+i, -i))) break;

		// rows & columns
		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(+i, 0))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(-i, 0))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(0, +i))) break;

		for (int i = 1;; i++)
			if (!CheckAndPush(pos + Position(0, -i))) break;
	}

	string GetName() const override
	{
		return "Queen";
	}
	PieceType GetType() const override
	{
		return PieceType::Queen;
	}
};


Piece* MakePawn(Position pos, PlayerTeam team, const ChessBoard* board)
{
	return new Pawn(pos, team, board);
}
Piece* MakeKnight(Position pos, PlayerTeam team, const ChessBoard* board)
{
	return new Knight(pos, team, board);
}
Piece* MakeBishop(Position pos, PlayerTeam team, const ChessBoard* board)
{
	return new Bishop(pos, team, board);
}
Piece* MakeRook(Position pos, PlayerTeam team, const ChessBoard* board)
{
	return new Rook(pos, team, board);
}
Piece* MakeQueen(Position pos, PlayerTeam team, const ChessBoard* board)
{
	return new Queen(pos, team, board);
}
Piece* MakeKing(Position pos, PlayerTeam team, const ChessBoard* board)
{
	return new King(pos, team, board);
}