#pragma once

#include "Coords.h"
#include "Other.h"
#include "Piece.h"
#include "Board.h"


class Pawn : public Piece
{
public:
	Pawn(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	vector<Position> GetMoves() const override
	{
		vector<Position> res;

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
			if (board->InBounds(newPos) && board->IsEmpty(pos + Position(0, dir)) && board->IsEmpty(newPos))
				res.push_back(newPos);
		}

		{
			Position newPos = pos + Position(0, dir);
			if (board->InBounds(newPos) && board->IsEmpty(newPos))
				res.push_back(newPos);
		}

		{
			Position newPos = Position(-1, dir);
			if (board->InBounds(newPos))
			{
				const Piece* p = board->GetPieceAt(newPos);

				if (p != nullptr && p->GetTeam() != GetTeam())
					res.push_back(newPos);
			}

			newPos = Position(+1, dir);
			if (board->InBounds(newPos))
			{
				const Piece* p = board->GetPieceAt(newPos);

				if (p != nullptr && p->GetTeam() != GetTeam())
					res.push_back(newPos);
			}
		}

		{
			// TODO: en passant
		}

		return res;
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

	vector<Position> GetMoves() const override
	{
		vector<Position> res;

		const int dx[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
		const int dy[] = { -2, -1, 1, 2, 2, 1, -1, -2 };

		for (int d = 0; d < 8; d++)
		{
			Position newPos = Position(pos.x + dx[d], pos.y + dy[d]);
			if (board->InBounds(newPos))
			{
				if (board->IsEmpty(newPos) || board->GetTeam(newPos) != GetTeam())
					res.push_back(newPos);
			}
		}

		return res;
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
public:
	Bishop(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	vector<Position> GetMoves() const override
	{
		vector<Position> res;

		for (int i = 0;; i++)
		{
			Position newPos = pos + Position(i, i);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(-i, -j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(-i, j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(i, -j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		return res;
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
public:
	bool moved;

	Rook(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board), moved(false) {}

	vector<Position> GetMoves() const override
	{
		vector<Position> res;

		for (int i = 0; i < 8; i++)
		{
			Position newPos = pos + Position(i, 0);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
		}

		for (int i = 0; i < 8; i++)
		{
			Position newPos = pos + Position(-i, 0);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
		}

		for (int j = 0; j < 8; j++)
		{
			Position newPos = pos + Position(0, j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
		}

		for (int j = 0; j < 8; j++)
		{
			Position newPos = pos + Position(0, -j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
		}

		return res;
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
	vector<Position> GetMoves() const override
	{
		vector<Position> res;

		const int dx[] = { 1, 1, -1, -1, 1, 0, -1, 0 };
		const int dy[] = { 1, -1, 1, -1, 0, 1, 0, -1 };

		for (int d = 0; d < 8; d++)
		{
			Position newPos = pos + Position(dx[d], dy[d]);
			if (board->InBounds(newPos))
			{
				if (board->IsEmpty(newPos) || board->GetTeam(newPos) != GetTeam())
					res.push_back(newPos);
			}
		}

		return res;
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
	Queen(Position pos, PlayerTeam team, const ChessBoard* board) : Piece(pos, team, board) {}

	vector<Position> GetMoves() const override
	{
		vector<Position> res;

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(i, j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(-i, -j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(-i, j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0, j = 0; i < 8 && j < 8; i++, j++)
		{
			Position newPos = pos + Position(i, -j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0; i < 8; i++)
		{
			Position newPos = pos + Position(i, 0);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int i = 0; i < 8; i++)
		{
			Position newPos = pos + Position(-i, 0);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int j = 0; j < 8; j++)
		{
			Position newPos = pos + Position(0, +j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		for (int j = 0; j < 8; j++)
		{
			Position newPos = pos + Position(0, -j);

			if (board->InBounds(newPos))
			{
				if (!board->IsEmpty(newPos))
				{
					if (board->GetTeam(newPos) != GetTeam())
						res.push_back(newPos);
					break;
				}
			}
			else
				break;
		}

		return res;
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