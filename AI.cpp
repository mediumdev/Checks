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

	//Проверяем есть ли фигура рядом со свободной клеткой базы противника, если есть выбираем ее для хода
	bestPiece = GetPiceNearFinishTiles(field);

	//Если фигура для хода не найдена
	if (bestPiece == nullptr)
	{
		//Проверяем есть ли своя фигура которая может ходить на базе противника, если есть выбираем ее для хода 
		//Это нужно для того чтобы упорядочить свои фигуры на базе противника
		bestPiece = GetPiceToMoveOnFinishTiles(field);
	}

	//Если фигура для хода не найдена
	if (bestPiece == nullptr)
	{
		//Подготавливаем веса для всех ходов
		PreparePiecesTurnWeights(field);

		//Собираем все ходы всех фигур в массив
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

		//Перемешиваем массив ходов
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(std::begin(positionWeights), std::end(positionWeights), std::default_random_engine(seed));

		//Сортируем ходы в массиве по весу
		std::sort(positionWeights.begin(), positionWeights.end(), [](const auto& a, const auto& b) {return a.weight < b.weight; });

		//Выбираем первый возможный ход с наименьшим весом
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

	//Ходим
	if (bestPiece != nullptr)
	{
		field.MovePiece(bestPiece->GetAiPositionToMove(), bestPiece);
	}

	//Передаем ход следующему игроку
	controller.NextPlayerTurn(field);
}

void AI::PreparePiecesTurnWeights(Field& field)
{
	//Для каждой своей фигуры рассчитываем вес каждого хода
	for (auto& piece : pieces)
	{
		//Рассчитываем веса ходов игнорируя фигуры на поле
		CalcPiecesTurnWeights(field, piece, true);
		//Рассчитываем веса ходов учитывая фигуры на поле
		//Эти ходы необходимо в тех случаях, когда другие фигуры мешают сделать ход по ближайшему пути
		CalcPiecesTurnWeights(field, piece, false);
	}
}

void AI::CalcPiecesTurnWeights(Field& field, std::shared_ptr<Piece> piece, bool ignorePieces)
{
	std::vector<Position> positions;
	std::vector<int> stepCounts;

	//Сохраняем длинну пути до базы противника каждой соседней клетки фигуры
	std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), true);
	for (auto& turnTale : turnTiles)
	{
		std::vector<Position> path = GetBestPath(turnTale->position, field, ignorePieces);

		positions.push_back(turnTale->position);
		stepCounts.push_back(path.size());
	}

	//Считаем вес каждого возможного хода для фигуры, в зависимости от длинны пути до базы противника
	piece->CalcTurnWeights(field, positions, stepCounts, ignorePieces);
}

std::shared_ptr<Piece> AI::GetPiceNearFinishTiles(Field& field)
{
	std::shared_ptr<Piece> bestPiece;

	for (auto& piece : pieces)
	{
		//Если фигура не находится на базе противника
		std::shared_ptr<Tile> pieceTile = field.GetTile(piece->GetPosition());
		if ((pieceTile->respawnPlayerNum != 0) && (pieceTile->respawnPlayerNum != playerNum))
		{
			continue;
		}

		//Ищем клетку рядом с фигурой, на которой появилась фигура противника
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
		//Если фигура находится на базе противника то переходим к следующей фигуре
		std::shared_ptr<Tile> tile = field.GetTile(piece->GetPosition());
		if ((tile->respawnPlayerNum == 0) || (tile->respawnPlayerNum == playerNum))
		{
			continue;
		}

		//Сохраняем вес клетки под фигурой
		int currentRespawnWeight = tile->respawnWeight;

		std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), true);
		//Сравниваем веса соседних клеток с той на которой находится фигура
		for (auto& turnTile : turnTiles)
		{
			//Если соседняя клетка не пустая переходим к следующей клетке 
			if (turnTile->piece != nullptr)
			{
				continue;
			}

			//Если вес клетки больше чем тот что у клетки под фигурой запоминаем фигуру и позицию клетки для хода
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
	//Расчитываем длину пути до каждой клетки базы противника и выбираем ближайший путь
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

	//Подготавливаем карту для поиска пути
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

	//Ищем путь
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

	//Путь не найден
	if ((map[endPosition.x][endPosition.y][0] == -1) || (step == 0))
	{
		result = false;
	}

	//Если путь найден, записываем его в вектор
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

	//Возвращаем путь
	return path;
}

AI::~AI()
{

}
