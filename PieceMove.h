#pragma once

#include <string>
#include <vector>
#include <ostream>

#include "Coords.h"
#include "Other.h"
#include "Piece.h"

using namespace std;


struct PieceMove
{
	PieceMove() : 
		type(), 
		captured(nullptr), 
		check(false), mate(false), 
		movedBefore(false), 
		piece(), 
		from(), to(), 
		promoted(nullptr), 
		notation()
	{}

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

	Piece* piece;
	Position from, to;

	Piece* promoted;

	string notation;
};