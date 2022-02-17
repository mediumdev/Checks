#include <algorithm>
#include "Piece.h"

Piece::Piece(Shade shade, Position position, int playerNum) : shade(shade), position(position), playerNum(playerNum)
{

}

int Piece::GetPlayerNum() const
{
	return playerNum;
}

void Piece::SetPosition(Position position)
{
	oldPosition = GetPosition();
	this->position = position;
}

Position Piece::GetPosition() const
{
	return position;
}

Position Piece::GetOldPosition() const
{
	return oldPosition;
}

Piece::~Piece()
{
	
}
