#include <algorithm>
#include "Piece.h"

Piece::Piece(Shade shade, Position position, int playerNum) : shade(shade), position(position), playerNum(playerNum)
{

}

int Piece::GetPlayerNum() const
{
	return playerNum;
}

void Piece::SetPosition(Position position)
{
	oldPosition = GetPosition();
	this->position = position;
}

Position Piece::GetPosition() const
{
	return position;
}

Position Piece::GetOldPosition() const
{
	return oldPosition;
}

void Piece::SetAiBestPath(std::vector<Position> aiOldPath)
{
	this->aiBestPath = aiOldPath;
}

std::vector<Position> Piece::GetAiBestPath() const
{
	return aiBestPath;
}

void Piece::CalcTurnWeights(Field& field, std::vector<Position> positions, std::vector<int> stepCounts, bool ignorePieces)
{
	if (positions.size() != stepCounts.size())
	{
		return;
	}

	if (positions.size() == 0)
	{
		return;
	}

	int weight = 0;
	int oldStepCount = 0;
	//Если количество шагов до базы противника без учета других фигур то очищаем вектор с весами
	if (ignorePieces)
	{
		positionWeights.clear();
	}
	//Если количество шагов до базы противника с учетом других фигур то начинаем отсчитывать вес с единицы
	else
	{
		weight = 1;
	}

	//Записываем все ходы от которых количество шагов до базы пративника не равно нулю
	std::vector<PositionWeight> preparePositionWeights;
	for (int i = 0; i < positions.size(); i++)
	{
		if (stepCounts[i] == 0)
		{
			continue;
		}
		preparePositionWeights.push_back(PositionWeight{ positions[i], stepCounts[i] });
	}

	//Сортируем ходы по количеству ходов до базы противника
	std::sort(preparePositionWeights.begin(), preparePositionWeights.end(), [](const auto& a, const auto& b) {return a.stepCounts < b.stepCounts; });

	//Назначаем вес каждому ходу
	for (auto& position : preparePositionWeights)
	{
		if (oldStepCount != position.stepCounts)
		{
			oldStepCount = position.stepCounts;
			weight += 2;
		}

		position.weight = weight;
		positionWeights.push_back(position);
	}
}

std::vector<PositionWeight> Piece::GetPositionWeights()
{
	return positionWeights;
}

void Piece::SetAiPositionToMove(Position position)
{
	aiPositionToMove = position;
}

Position Piece::GetAiPositionToMove() const
{
	return aiPositionToMove;
}

Piece::~Piece()
{
	
}
