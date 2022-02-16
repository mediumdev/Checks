#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Field.h"
#include "Piece.h"

class View;
class Controller;

class Player
{
public:
	Player(int playerNum, Shade shade);
	Shade GetShade() const;
	int GetPlayerNum() const;
	std::shared_ptr<Piece> AddPiece(Position position);
	std::shared_ptr<Piece> GetPiece(int index) const;
	int GetPiecesCount() const;
	void ClearPieces();
	void CheckBaseIsFull(Field& field);
	bool GetBaseIsFull() const;
	virtual void Prepare(Field& field);
	virtual void Turn(const View& view, Controller& controller, Field& field);
	virtual ~Player();
private:
	Shade shade = Shade::LIGHT;
	int piecesCount = 0;
	bool baseIsFull = false;
protected:
	std::vector<std::shared_ptr<Piece>> pieces;
	int playerNum = 0;
	std::shared_ptr<Piece> pieceSelected = nullptr;
};

