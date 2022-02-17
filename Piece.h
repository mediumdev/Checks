#pragma once
#include "Field.h"

class Piece
{
public:
	Piece(Shade shade, Position position, int playerNum);
	int GetPlayerNum() const;
	void SetPosition(Position position);
	Position GetPosition() const;
	Position GetOldPosition() const;
	~Piece();
private:

	Position position{ 0, 0 };
	Position oldPosition{ -1, -1 };
	Shade shade = Shade::LIGHT;
	int playerNum = 0;
};