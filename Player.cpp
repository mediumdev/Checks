#include "Player.h"

Player::Player(int playerNum, Shade shade) : playerNum(playerNum), shade(shade)
{
	piecesCount = 0;
}

Shade Player::GetShade() const
{
	return shade;
}

int Player::GetPlayerNum() const
{
	return playerNum;
}

std::shared_ptr<Piece> Player::AddPiece(Position position)
{
	pieces.push_back(std::make_shared<Piece>(shade, position, playerNum));
	piecesCount = pieces.size();
	return pieces.back();
}

std::shared_ptr<Piece> Player::GetPiece(int index) const
{
	if ((index >= 0) && (index < int (pieces.size())))
	{
		return pieces[index];
	}
	return {};
}

int Player::GetPiecesCount() const
{
	return piecesCount;
}

void Player::ClearPieces()
{
	pieces.clear();
}

void Player::CheckBaseIsFull(Field& field)
{
	std::vector<std::shared_ptr<Tile>> finishTiles = field.GetFinishTiles(playerNum);
	int finishTileCount = finishTiles.size();
	int frendlyPiecesCount = 0;
	int enemyPiecesCount = 0;

	for (auto& finishTile : finishTiles)
	{
		if (finishTile->piece == nullptr)
		{
			continue;
		}

		if (finishTile->piece->GetPlayerNum() == playerNum)
		{
			frendlyPiecesCount++;
		}
		else
		{
			std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(finishTile->position, false);
			if (turnTiles.size() == 0)
			{
				enemyPiecesCount++;
			}
		}
	}

	if (frendlyPiecesCount + enemyPiecesCount == finishTiles.size())
	{
		baseIsFull = true;
	}
}

bool Player::GetBaseIsFull() const
{
	return baseIsFull;
}



void Player::Prepare(Field& field)
{

}

void Player::Turn(const View& view, Controller& controller, Field& field)
{

}

Player::~Player()
{

}
