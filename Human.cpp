#include "Human.h"
#include "View.h"
#include "Controller.h"

Human::Human(int playerNum, Shade shade) : Player(playerNum, shade)
{

}

void Human::Prepare(Field& field)
{

}

void Human::Turn(const View& view, Controller& controller, Field& field)
{
	Position mousePosition = view.GetMousePosition();

	if ((mousePosition.x < 0) ||
		(mousePosition.y < 0) ||
		(mousePosition.x >= field.GetWidth() * field.GetTileSize()) ||
		(mousePosition.y >= field.GetHeight() * field.GetTileSize()))
	{
		return;
	}

	if ((view.GetLeftMousePressed()) && (!mouseIsDown))
	{
		mouseIsDown = true;
		mouseClick = true;
	}

	if (!view.GetLeftMousePressed())
	{
		mouseIsDown = false;
	}

	if (mouseClick)
	{
		mouseClick = false;
		std::shared_ptr<Tile> tile = field.GetTile(mousePosition.x / field.GetTileSize(), mousePosition.y / field.GetTileSize());

		if (tile->availableForTurn)
		{
			field.MovePiece(tile->position, pieceSelected);
			controller.NextPlayerTurn(field);
			isTurn = true;
		}

		for (auto& turnTile : turnTales)
		{
			std::shared_ptr<Tile> tile = field.GetTile(turnTile->position);
			tile->availableForTurn = false;
		}

		if (isTurn)
		{
			isTurn = false;
			return;
		}

		if (tile->piece == nullptr)
		{
			return;
		}

		if (tile->piece->GetPlayerNum() != playerNum)
		{
			return;
		}

		pieceSelected = tile->piece;

		turnTales = field.GetTilesForTurn(tile->position, false);

		if (turnTales.size() <= 0)
		{
			return;
		}

		for (auto& turnTile : turnTales)
		{
			std::shared_ptr<Tile> tile = field.GetTile(turnTile->position);
			tile->availableForTurn = true;
		}
	}
}

Human::~Human()
{

}
