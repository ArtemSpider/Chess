#pragma once

#include <string>

#include "Coords.h"
#include "Other.h"

using namespace std;

struct PieceMove
{
	enum class MoveType
	{
		Move,
		CastleShort,
		CastleLong,
		PromotionKnight,
		PromotionBishop,
		PromotionRook,
		PromotionQueen
	};
	MoveType type;

	bool capture;
	bool check, mate;

	PieceType piece;
	Position from, to;

	/*
	string AsString()
	{
		if (type == MoveType::CastleLong)
			return "o-o-o";
		if (type == MoveType::CastleShort)
			return "o-o";

		string res;

		if (piece == PieceType::Pawn)
		{
			if (capture)
				res = string(1, 'a' + from.x) + "x";
			res += ToChessNotation(to);
		}
		else
		{
			switch (piece)
			{
			case PieceType::Knight: res += 'N'; break;
			case PieceType::Bishop: res += 'B'; break;
			case PieceType::Rook: res += 'R'; break;
			case PieceType::Queen: res += 'Q'; break;
			case PieceType::King: res += 'K'; break;
			}

			if (capture)
				res += 'x';
		}
	}*/
};