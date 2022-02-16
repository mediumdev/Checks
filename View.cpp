#include <SFML/Graphics.hpp>
#include "View.h"
#include "Field.h"
#include "Player.h"

struct Tile;
enum class PopupType;

struct WinnerPopup
{
	bool isShow = false;
	PopupType type = PopupType::DRAW;
	std::vector<Shade> shades;
};

struct ViewProxy
{
	sf::Texture lightTileTex;
	sf::Texture darkTileTex;
	sf::Texture lightPieceTex;
	sf::Texture darkPieceTex;
	sf::Texture squareTex;
	sf::Texture popupWinTex;
	sf::Texture popupDrawTex;

	sf::Sprite lightTile;
	sf::Sprite darkTile;
	sf::Sprite lightPiece;
	sf::Sprite darkPiece;
	sf::Sprite square;
	sf::Sprite popupWin;
	sf::Sprite popupDraw;

	std::unique_ptr<sf::RenderWindow> window;
};

View::View() : viewProxy{ std::make_unique<ViewProxy>() }, winnerPopup{ std::make_unique<WinnerPopup>() }
{
	viewProxy->window = std::make_unique<sf::RenderWindow>(sf::VideoMode(512, 512), "Denis Test Task", sf::Style::Close);
}

bool View::LoadResources()
{
	//load textures
	if (!viewProxy->lightTileTex.loadFromFile("images/light_tile.png"))
	{
		return false;
	}

	if (!viewProxy->darkTileTex.loadFromFile("images/dark_tile.png"))
	{
		return false;
	}

	if (!viewProxy->lightPieceTex.loadFromFile("images/light_piece.png"))
	{
		return false;
	}

	if (!viewProxy->darkPieceTex.loadFromFile("images/dark_piece.png"))
	{
		return false;
	}

	if (!viewProxy->squareTex.loadFromFile("images/square.png"))
	{
		return false;
	}

	if (!viewProxy->popupWinTex.loadFromFile("images/popup_win.png"))
	{
		return false;
	}

	if (!viewProxy->popupDrawTex.loadFromFile("images/popup_draw.png"))
	{
		return false;
	}

	//create sprites
	viewProxy->lightTile.setTexture(viewProxy->lightTileTex);
	viewProxy->darkTile.setTexture(viewProxy->darkTileTex);
	viewProxy->lightPiece.setTexture(viewProxy->lightPieceTex);
	viewProxy->darkPiece.setTexture(viewProxy->darkPieceTex);
	viewProxy->square.setTexture(viewProxy->squareTex);
	viewProxy->popupWin.setTexture(viewProxy->popupWinTex);
	viewProxy->popupDraw.setTexture(viewProxy->popupDrawTex);

	return true;
}

void View::EventHandler()
{
	sf::Event event;
	while (viewProxy->window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			viewProxy->window->close();
	}
}

void View::Draw(const Field& field, const std::vector<std::shared_ptr<Player>>& players)
{
	viewProxy->window->clear();

	//draw field
	for (int i = 0; i < field.GetWidth(); i++)
	{
		for (int j = 0; j < field.GetHeight(); j++)
		{
			Position position{ i, j };
			int x = i * field.GetTileSize();
			int y = j * field.GetTileSize();

			auto& sprite = field.GetTile(position)->shade == Shade::LIGHT ? viewProxy->lightTile : viewProxy->darkTile;
			sprite.setPosition(float(x), float(y));

			viewProxy->window->draw(sprite);

			if (field.GetTile(position)->availableForTurn)
			{
				auto& square = viewProxy->square;
				square.setPosition(float(x), float(y));
				viewProxy->window->draw(square);
			}
		}
	}

	//draw piecess
	for (auto& player : players)
	{
		for (int i = 0; i < player->GetPiecesCount(); i++)
		{
			if (auto piece = player->GetPiece(i))
			{
				auto& sprite = player->GetShade() == Shade::LIGHT ? viewProxy->lightPiece : viewProxy->darkPiece;
				sprite.setPosition(
					float(piece->GetPosition().x * field.GetTileSize()),
					float(piece->GetPosition().y * field.GetTileSize()));

				viewProxy->window->draw(sprite);
			}
		}
	}

	if (winnerPopup->isShow)
	{
		auto& background = winnerPopup->type == PopupType::DRAW ? viewProxy->popupDraw : viewProxy->popupWin;
		const auto& windowSize = viewProxy->window->getSize();
		background.setPosition(float(windowSize.x / 2 - background.getTextureRect().width / 2), float(windowSize.y / 2 - background.getTextureRect().height / 2));
		viewProxy->window->draw(background);

		switch (winnerPopup->type)
		{
		case PopupType::DRAW:
			break;

		case PopupType::WIN:
			for (auto& shade : winnerPopup->shades)
			{
				auto& sprite = shade == Shade::LIGHT ? viewProxy->lightPiece : viewProxy->darkPiece;
				sprite.setPosition(float(windowSize.x / 2 - field.GetTileSize() / 2), float(windowSize.y / 2 - field.GetTileSize() / 2));
				viewProxy->window->draw(sprite);
			}
			break;

		}
	}

	viewProxy->window->display();
}

bool View::WindowsIsOpen() const
{
	return viewProxy->window->isOpen();
}

Position View::GetMousePosition() const
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(*viewProxy->window);
	Position position{ mousePosition.x, mousePosition.y };
	return position;
}

bool View::GetLeftMousePressed() const
{
	return sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

bool View::GetSpacePressed() const
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
}

void View::ShowWinnerPopup(PopupType popupType, const std::vector<std::shared_ptr<Player>> winners)
{
	winnerPopup->type = popupType;
	winnerPopup->shades.clear();

	for (auto& winner : winners)
	{
		winnerPopup->shades.push_back(winner->GetShade());
	}

	winnerPopup->isShow = true;
}

void View::HideWinnerPopup()
{
	winnerPopup->isShow = false;
}

const bool View::GetWinnerPopupIsShow() const
{
	return winnerPopup->isShow;
}

View::~View()
{

}

