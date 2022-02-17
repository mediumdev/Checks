#include <string>
#include "Controller.h"
#include "Field.h"
#include "Player.h"
#include "Human.h"
#include "AI.h"
#include "View.h"

enum class PopupType;

Controller::Controller(std::vector<std::shared_ptr<Player>>& players) : players(players)
{
	
}

void Controller::StartNewGame(Field& field)
{
	winners.clear();

	players.clear();
	players.push_back(std::make_shared<Human>(1, Shade::LIGHT));
	players.push_back(std::make_shared<AI>(2, Shade::DARK));

	field.Clear();
	field.SetRespawnRect(5, 5, 3, 3, 1);
	field.SetRespawnRect(0, 0, 3, 3, 2);

	for (auto& player : players)
	{
		std::vector<std::shared_ptr<Tile>> tiles = field.GetRespawn(player->GetPlayerNum());
		player->ClearPieces();
		for (auto& tile : tiles)
		{
			field.MovePiece(tile->position, player->AddPiece(tile->position));
		}
	}

	currentPlayerIndex = 0;
	players[currentPlayerIndex]->Prepare(field);
}

void Controller::Update(View& view, Field& field)
{
	if (!view.GetWinnerPopupIsShow())
	{
		players[currentPlayerIndex]->Turn(view, *this, field);

		if (winners.size() == 0)
		{
			return;
		}

		if (winners.size() == players.size())
		{
			view.ShowWinnerPopup(PopupType::DRAW, winners);
		}
		else
		{
			view.ShowWinnerPopup(PopupType::WIN, winners);
		}
	}
	else
	{
		if (view.GetSpacePressed())
		{
			view.HideWinnerPopup();
			StartNewGame(field);
		}
	}
}

void Controller::NextPlayerTurn(Field& field)
{
	if (players.size() == 0)
	{
		return;
	}

	currentPlayerIndex++;

	if (currentPlayerIndex == players.size())
	{
		CheckWinner(field);
		currentPlayerIndex = 0;
	}

	players[currentPlayerIndex]->Prepare(field);
}

void Controller::CheckWinner(Field& field)
{
	winners.clear();

	for (auto& player : players)
	{
		if (player->CheckBaseIsFull(field))
		{
			winners.push_back(player);
		}
	}
}

Controller::~Controller()
{

}
