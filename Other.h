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

	Count // Must be last
};