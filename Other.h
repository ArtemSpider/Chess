#pragma once

#include <string>

#include "Coords.h"

using namespace std;


enum class PlayerTeam
{
	White,
	Black
};
PlayerTeam OtherTeam(PlayerTeam team)
{
	return (team == PlayerTeam::White ? PlayerTeam::Black : PlayerTeam::White);
}

enum class PieceType
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King,

	Count = 6 // Must be last
};


string ToChessNotation(Position pos)
{
	return string(1, 'a' + pos.x) + string(1, '1' + pos.y);
}