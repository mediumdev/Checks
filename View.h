#pragma once
#include <vector>
#include <memory>

namespace sf
{
	class RenderWindow;
	class Mouse;
}

struct ViewProxy;
class Field;
class Player;
struct Position;
struct WinnerPopup;
enum class PopupType;

class View
{
public:
	View();
	bool LoadResources();
	void EventHandler();
	void Draw(const Field& field, const std::vector<std::shared_ptr<Player>>& players);
	bool WindowsIsOpen() const;
	Position GetMousePosition() const;
	bool GetLeftMousePressed() const;
	bool GetSpacePressed() const;
	void ShowTurn(std::vector<Position> positions);
	void ShowWinnerPopup(PopupType popupType, const std::vector<std::shared_ptr<Player>>& winners);
	void HideWinnerPopup();
	const bool GetWinnerPopupIsShow() const;
	~View();
private:
	std::unique_ptr<ViewProxy> viewProxy;
	std::unique_ptr<WinnerPopup> winnerPopup;

	std::vector<Position> turnPositions;
};

enum class PopupType
{
	DRAW, WIN
};