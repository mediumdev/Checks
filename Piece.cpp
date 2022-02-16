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
	//���� ���������� ����� �� ���� ���������� ��� ����� ������ ����� �� ������� ������ � ������
	if (ignorePieces)
	{
		positionWeights.clear();
	}
	//���� ���������� ����� �� ���� ���������� � ������ ������ ����� �� �������� ����������� ��� � �������
	else
	{
		weight = 1;
	}

	//���������� ��� ���� �� ������� ���������� ����� �� ���� ���������� �� ����� ����
	std::vector<PositionWeight> preparePositionWeights;
	for (int i = 0; i < positions.size(); i++)
	{
		if (stepCounts[i] == 0)
		{
			continue;
		}
		preparePositionWeights.push_back(PositionWeight{ positions[i], stepCounts[i] });
	}

	//��������� ���� �� ���������� ����� �� ���� ����������
	std::sort(preparePositionWeights.begin(), preparePositionWeights.end(), [](const auto& a, const auto& b) {return a.stepCounts < b.stepCounts; });

	//��������� ��� ������� ����
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
