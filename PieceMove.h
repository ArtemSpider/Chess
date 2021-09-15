#pragma once

#include <string>

#include "Coords.h"
#include "Other.h"

using namespace std;


struct PieceMove
{
	PieceMove() : type(), capture(false), check(false), mate(false), piece(), from(), to() {}

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
};