#include <random>
#include <chrono>
#include <algorithm>
#include "AI.h"
#include "Controller.h"

AI::AI(int playerNum, Shade shade) : Player(playerNum, shade)
{

}

void AI::Prepare(Field& field)
{

}

void AI::Turn(const View& view, Controller& controller, Field& field)
{
	std::shared_ptr<Piece> bestPiece;

	//��������� ���� �� ������ ����� �� ��������� ������� ���� ����������, ���� ���� �������� �� ��� ����
	bestPiece = GetPiceNearFinishTiles(field);

	//���� ������ ��� ���� �� �������
	if (bestPiece == nullptr)
	{
		//��������� ���� �� ���� ������ ������� ����� ������ �� ���� ����������, ���� ���� �������� �� ��� ���� 
		//��� ����� ��� ���� ����� ����������� ���� ������ �� ���� ����������
		bestPiece = GetPiceToMoveOnFinishTiles(field);
	}

	//���� ������ ��� ���� �� �������
	if (bestPiece == nullptr)
	{
		//�������������� ���� ��� ���� �����
		PreparePiecesTurnWeights(field);

		//�������� ��� ���� ���� ����� � ������
		std::vector<PositionWeight> positionWeights;
		for (auto& piece : pieces)
		{
			std::shared_ptr<Tile> pieceTile = field.GetTile(piece->GetPosition());
			if ((pieceTile->respawnPlayerNum != playerNum) && (pieceTile->respawnPlayerNum != 0))
			{
				continue;
			}

			std::vector<PositionWeight> weights = piece->GetPositionWeights();
			if (weights.size() == 0)
			{
				continue;
			}

			for (auto& weight : weights)
			{
				weight.piece = piece;
				positionWeights.push_back(weight);
			}
		}

		//������������ ������ �����
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(std::begin(positionWeights), std::end(positionWeights), std::default_random_engine(seed));

		//��������� ���� � ������� �� ����
		std::sort(positionWeights.begin(), positionWeights.end(), [](const auto& a, const auto& b) {return a.weight < b.weight; });

		//�������� ������ ��������� ��� � ���������� �����
		for (auto& positionWeight : positionWeights)
		{
			std::shared_ptr<Tile> tile = field.GetTile(positionWeight.position);
			if (tile->piece != nullptr)
			{
				continue;
			}

			Position oldPosition = positionWeight.piece->GetOldPosition();
			if ((positionWeight.position.x == oldPosition.x) && (positionWeight.position.y == oldPosition.y))
			{
				continue;
			}

			bestPiece = positionWeight.piece;
			bestPiece->SetAiPositionToMove(positionWeight.position);
			break;
		}
	}

	//�����
	if (bestPiece != nullptr)
	{
		field.MovePiece(bestPiece->GetAiPositionToMove(), bestPiece);
	}

	//�������� ��� ���������� ������
	controller.NextPlayerTurn(field);
}

void AI::PreparePiecesTurnWeights(Field& field)
{
	//��� ������ ����� ������ ������������ ��� ������� ����
	for (auto& piece : pieces)
	{
		//������������ ���� ����� ��������� ������ �� ����
		CalcPiecesTurnWeights(field, piece, true);
		//������������ ���� ����� �������� ������ �� ����
		//��� ���� ���������� � ��� �������, ����� ������ ������ ������ ������� ��� �� ���������� ����
		CalcPiecesTurnWeights(field, piece, false);
	}
}

void AI::CalcPiecesTurnWeights(Field& field, std::shared_ptr<Piece> piece, bool ignorePieces)
{
	std::vector<Position> positions;
	std::vector<int> stepCounts;

	//��������� ������ ���� �� ���� ���������� ������ �������� ������ ������
	std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), true);
	for (auto& turnTale : turnTiles)
	{
		std::vector<Position> path = GetBestPath(turnTale->position, field, ignorePieces);

		positions.push_back(turnTale->position);
		stepCounts.push_back(path.size());
	}

	//������� ��� ������� ���������� ���� ��� ������, � ����������� �� ������ ���� �� ���� ����������
	piece->CalcTurnWeights(field, positions, stepCounts, ignorePieces);
}

std::shared_ptr<Piece> AI::GetPiceNearFinishTiles(Field& field)
{
	std::shared_ptr<Piece> bestPiece;

	for (auto& piece : pieces)
	{
		//���� ������ �� ��������� �� ���� ����������
		std::shared_ptr<Tile> pieceTile = field.GetTile(piece->GetPosition());
		if ((pieceTile->respawnPlayerNum != 0) && (pieceTile->respawnPlayerNum != playerNum))
		{
			continue;
		}

		//���� ������ ����� � �������, �� ������� ��������� ������ ����������
		std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), false);
		for (auto& turnTile : turnTiles)
		{
			if ((turnTile->respawnPlayerNum != 0) && (turnTile->respawnPlayerNum != playerNum))
			{
				bestPiece = piece;
				bestPiece->SetAiPositionToMove(turnTile->position);
				break;
			}
		}
	}

	return bestPiece;
}

std::shared_ptr<Piece> AI::GetPiceToMoveOnFinishTiles(Field& field) const
{
	std::shared_ptr<Piece> bestPiece;

	for (auto& piece : pieces)
	{
		//���� ������ ��������� �� ���� ���������� �� ��������� � ��������� ������
		std::shared_ptr<Tile> tile = field.GetTile(piece->GetPosition());
		if ((tile->respawnPlayerNum == 0) || (tile->respawnPlayerNum == playerNum))
		{
			continue;
		}

		//��������� ��� ������ ��� �������
		int currentRespawnWeight = tile->respawnWeight;

		std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), true);
		//���������� ���� �������� ������ � ��� �� ������� ��������� ������
		for (auto& turnTile : turnTiles)
		{
			//���� �������� ������ �� ������ ��������� � ��������� ������ 
			if (turnTile->piece != nullptr)
			{
				continue;
			}

			//���� ��� ������ ������ ��� ��� ��� � ������ ��� ������� ���������� ������ � ������� ������ ��� ����
			if (turnTile->respawnWeight > currentRespawnWeight)
			{
				bestPiece = piece;
				bestPiece->SetAiPositionToMove(turnTile->position);
				continue;
			}
		}
	}

	return bestPiece;
}

std::vector<Position> AI::GetBestPath(Position position, Field& field, bool ignorePieces)
{
	std::vector<Position> bestPath;
	std::vector<std::shared_ptr<Tile>> finishTiles = field.GetFinishTiles(playerNum);
	std::sort(finishTiles.begin(), finishTiles.end(), [](const auto& a, const auto& b) {return a->respawnWeight < b->respawnWeight; });

	int stepCount = std::numeric_limits<int>::max();
	//����������� ����� ���� �� ������ ������ ���� ���������� � �������� ��������� ����
	for (auto& tile : finishTiles)
	{
		if ((tile->piece != nullptr) && (tile->piece->GetPlayerNum() == playerNum))
		{
			continue;
		}

		std::vector<Position> path = GetPath(position, tile->position, field, ignorePieces);
		if (path.size() > stepCount)
		{
			continue;
		}

		if (path.size() == 0)
		{
			continue;
		}

		stepCount = path.size();
		bestPath = path;
	}

	return bestPath;
}

std::vector<Position> AI::GetPath(Position startPosition, Position endPosition, Field& field, bool ignorePieces)
{
	const int width = field.GetWidth();
	const int height = field.GetHeight();

	std::vector<std::vector<std::vector<int>>> map;

	//�������������� ����� ��� ������ ����
	map.resize(width);
	for (int i = 0; i < width; i++)
	{
		map[i].resize(height);
		for (int j = 0; j < height; j++)
		{
			map[i][j].resize(3, 0);

			std::shared_ptr<Tile> tile = field.GetTile(i, j);
			if (tile->piece != nullptr)
			{
				map[i][j][0] = -2;

				if (ignorePieces)
				{
					if (tile->piece->GetPlayerNum() != 0)
					{
						map[i][j][0] = -1;
					}
				}
			}
			else
			{
				map[i][j][0] = -1;
			}
		}
	}

	int step = 0;
	bool added = true;
	bool result = true;

	std::vector<Position> path;

	map[endPosition.x][endPosition.y][0] = -1;
	map[startPosition.x][startPosition.y][0] = 0;

	//���� ����
	while (added && map[endPosition.x][endPosition.y][0] == -1)
	{
		added = false;
		step++;
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if (map[i][j][0] == step - 1)
				{
					int _i, _j;

					_i = i + 1; _j = j;
					if (_i >= 0 && _j >= 0 && _i < width && _j < height)
					{
						if (map[_i][_j][0] == -1 && map[_i][_j][0] != -2)
						{
							map[_i][_j][0] = step;
							map[_i][_j][1] = i;
							map[_i][_j][2] = j;
							added = true;
						}
					}
					_i = i - 1; _j = j;
					if (_i >= 0 && _j >= 0 && _i < width && _j < height)
					{
						if (map[_i][_j][0] == -1 && map[_i][_j][0] != -2)
						{
							map[_i][_j][0] = step;
							map[_i][_j][1] = i;
							map[_i][_j][2] = j;
							added = true;
						}
					}
					_i = i; _j = j + 1;
					if (_i >= 0 && _j >= 0 && _i < width && _j < height)
					{
						if (map[_i][_j][0] == -1 && map[_i][_j][0] != -2)
						{
							map[_i][_j][0] = step;
							map[_i][_j][1] = i;
							map[_i][_j][2] = j;
							added = true;
						}
					}
					_i = i; _j = j - 1;
					if (_i >= 0 && _j >= 0 && _i < width && _j < height)
					{
						if (map[_i][_j][0] == -1 && map[_i][_j][0] != -2)
						{
							map[_i][_j][0] = step;
							map[_i][_j][1] = i;
							map[_i][_j][2] = j;
							added = true;
						}
					}
				}
			}
		}
	}

	//���� �� ������
	if ((map[endPosition.x][endPosition.y][0] == -1) || (step == 0))
	{
		result = false;
	}

	//���� ���� ������, ���������� ��� � ������
	if (result) 
	{
		int _i = endPosition.x, _j = endPosition.y;

		while (map[_i][_j][0] != 0)
		{
			std::shared_ptr<Tile> tile = field.GetTile(_i, _j);
			path.push_back(tile->position);
			int li = map[_i][_j][1];
			int lj = map[_i][_j][2];
			_i = li; _j = lj;
		}
	}

	//���������� ����
	return path;
}

AI::~AI()
{

}
