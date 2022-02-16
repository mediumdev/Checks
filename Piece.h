#pragma once
#include "Field.h"

struct PositionWeight
{
	Position position{ 0, 0 };
	int stepCounts = 0;
	int weight = 0;
	std::shared_ptr<Piece> piece;
};

class Piece
{
public:
	Piece(Shade shade, Position position, int playerNum);
	int GetPlayerNum() const;
	void SetPosition(Position position);
	Position GetPosition() const;
	Position GetOldPosition() const;
	void SetAiBestPath(std::vector<Position> aiOldPath);
	std::vector<Position> GetAiBestPath() const;
	void CalcTurnWeights(Field& field, std::vector<Position> positions, std::vector<int> stepCounts, bool ignorePieces);
	std::vector<PositionWeight> GetPositionWeights();
	void SetAiPositionToMove(Position position);
	Position GetAiPositionToMove() const;
	~Piece();
private:

	Position position{ 0, 0 };
	Position oldPosition{ -1, -1 };
	Shade shade = Shade::LIGHT;
	int playerNum = 0;
	std::vector<Position> aiBestPath;
	Position aiPositionToMove{ 0, 0 };
	std::vector<PositionWeight> positionWeights;
};