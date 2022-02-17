#include "Human.h"
#include "View.h"
#include "Controller.h"

Human::Human(int playerNum, Shade shade) : Player(playerNum, shade)
{

}

void Human::Prepare(Field& field)
{

}

void Human::Turn(View& view, Controller& controller, Field& field)
{
	Position mousePosition = view.GetMousePosition();
	//ѕровер€ем находитс€ ли курсор в пределах доски
	if ((mousePosition.x < 0) ||
		(mousePosition.y < 0) ||
		(mousePosition.x >= field.GetWidth() * field.GetTileSize()) ||
		(mousePosition.y >= field.GetHeight() * field.GetTileSize()))
	{
		return;
	}

	//≈сли мышка нажата но не отжата, то происходит клик
	if ((view.GetLeftMousePressed()) && (!mouseIsDown))
	{
		mouseIsDown = true;
		mouseClick = true;
	}

	//≈сли мышка отжата, то можно кликнуть еще раз
	if (!view.GetLeftMousePressed())
	{
		mouseIsDown = false;
	}

	//ѕри клике мышкой
	if (mouseClick)
	{
		mouseClick = false;

		std::shared_ptr<Tile> tileSelected = field.GetTile(mousePosition.x / field.GetTileSize(), mousePosition.y / field.GetTileSize());
		std::shared_ptr<Piece> tilePice = tileSelected->piece;

		//≈сли фигура выбрана и кликнули на клетку доступную дл€ хода, то ходим фигурой
		if (pieceSelected != nullptr)
		{
			std::vector<std::shared_ptr<Tile>> tTiles = field.GetTilesForTurn(pieceSelected->GetPosition(), false);
			for (auto& tile : tTiles)
			{
				if ((tile->position.x == tileSelected->position.x) && (tile->position.y == tileSelected->position.y))
				{
					field.MovePiece(tile->position, pieceSelected);
					pieceSelected = nullptr;
					controller.NextPlayerTurn(field);
					break;
				}
			}
		}

		//≈сли кликнули на пустую клетку или на клетку с вражеской фигурой, то очищаем выбранную фигуру
		if ((tilePice == nullptr) || (tilePice->GetPlayerNum() != playerNum))
		{
			pieceSelected = nullptr;
		}
		//»наче записываем фигуру на клетке как выбранную
		else
		{
			pieceSelected = tilePice;
		}

		//≈сли фигура выбрана, то записуем клетки на которые она может походить
		std::vector<std::shared_ptr<Tile>> turnTiles;
		if (pieceSelected != nullptr)
		{
			turnTiles = field.GetTilesForTurn(tileSelected->position, false);
		}

		//¬ыбираем позиции клеток на которые может походить фигура
		std::vector<Position> turnPositions;
		for (auto& tile : turnTiles)
		{
			turnPositions.push_back(tile->position);
		}

		//”казываем что клетки на которые может походить фигура нужно выделить на экране
		view.ShowTurn(turnPositions);
	}
}

Human::~Human()
{

}
