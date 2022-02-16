#pragma once
#include <memory>
#include <vector>

class View;
class Field;
class Player;

class Controller
{
public:
	Controller() = delete;
	Controller(std::vector<std::shared_ptr<Player>>& players);
	void StartNewGame(Field& field);
	void Update(View& view, Field& field);
	void NextPlayerTurn(Field& field);
	~Controller();
private:
	void CheckWinner(Field& field);

	int currentPlayerIndex = 0;
	std::shared_ptr<Field> field = nullptr;
	std::vector<std::shared_ptr<Player>> winners;
	std::vector<std::shared_ptr<Player>>& players;
};

