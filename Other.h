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
	Pawn = 0,
	Knight,
	Bishop,
	Rook,
	Queen,
	King,

	Count // Must be last
};

string FileToNotation(int file)
{
	return string(1, 'a' + file);
}
string FileToNotation(Position pos)
{
	return string(1, 'a' + pos.x);
}

string RankToNotation(int rank)
{
	return string(1, '1' + rank);
}
string RankToNotation(Position pos)
{
	return string(1, '1' + pos.y);
}

string ToNotation(Position pos)
{
	return string(1, 'a' + pos.x) + string(1, '1' + pos.y);
}
Position FromNotation(string notation)
{
	return Position(notation[0] - 'a', notation[1] - '1');
}