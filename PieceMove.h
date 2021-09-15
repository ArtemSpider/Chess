#pragma once

#include <string>

#include "Coords.h"
#include "Other.h"
#include "Piece.h"

using namespace std;


struct PieceMove
{
	PieceMove() : type(), captured(nullptr), check(false), mate(false), movedBefore(false), piece(), from(), to() {}

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

	Piece* captured;
	bool check, mate;
	bool movedBefore;

	PieceType piece;
	Position from, to;
};