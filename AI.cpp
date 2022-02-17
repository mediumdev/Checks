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

void AI::Turn(View& view, Controller& controller, Field& field)
{
	std::shared_ptr<PositionPriority> bestTurn;

	//Проверяем есть ли фигура рядом со свободной клеткой базы противника, если есть выбираем ее для хода
	bestTurn = GetPieceNearFinishTiles(field);

	//Если фигура для хода не найдена
	if (bestTurn == nullptr)
	{
		//Проверяем есть ли фигура которая может ходить на базе противника, если есть выбираем ее для хода 
		//Это нужно для того чтобы упорядочить фигуры на базе противника
		bestTurn = GetPieceToMoveOnFinishTiles(field);
	}

	//Если фигура для хода не найдена
	if (bestTurn == nullptr)
	{
		//Очищаем список возможных ходов
		ClearPositionPriorities();

		for (auto& piece : pieces)
		{
			//Проверяем что фигура находится не на базе противника
			std::shared_ptr<Tile> pieceTile = field.GetTile(piece->GetPosition());
			if (field.СheckEnemyBaseTile(pieceTile, playerNum))
			{
				continue;
			}

			//Выставляем приоритет ходам с наименьшим путем до базы противника и добавляем в список возможных ходов
			//(путь просчитывается без учета других фигур)
			std::vector<std::shared_ptr<PositionPriority>> turnTilesPathIgnorePieces = GetTurnTilesPath(field, piece, true);
			CalcAndPushTurnTilesPriority(turnTilesPathIgnorePieces, false);
			//Выставляем приоритет ходам с наименьшим путем до базы противника и добавляем в список возможных ходов
			//(путь просчитывается с учетом других фигур)
			std::vector<std::shared_ptr<PositionPriority>> turnTilesPathNotIgnorePieces = GetTurnTilesPath(field, piece, false);
			CalcAndPushTurnTilesPriority(turnTilesPathNotIgnorePieces, true);
		}

		//Перемешиваем список возможных ходов и сортируем ходы по приоритету
		SortPositionPriorities();

		//Первый возможный ход из списка ходов записываем как лучший
		bestTurn = GetBestTurn(field);
	}

	//Ходим
	field.MovePiece(bestTurn->position, bestTurn->piece);

	//Передаем ход следующему игроку
	controller.NextPlayerTurn(field);
}

std::vector<std::shared_ptr<PositionPriority>> AI::GetTurnTilesPath(Field& field, std::shared_ptr<Piece> piece, bool ignorePieces)
{
	std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), true);
	std::vector<std::shared_ptr<PositionPriority>> piecePositionPriorities;

	//Находим клетки на которые может походить фигура
	for (auto& turnTale : turnTiles)
	{
		std::vector<Position> path = GetBestPath(turnTale->position, field, ignorePieces);

		//Если пути нет, то переходим к следующей клетке
		if (path.size() == 0)
		{
			continue;
		}

		//Записываем клетку как возможную для хода
		piecePositionPriorities.push_back(std::make_shared<PositionPriority>(piece, turnTale->position, int (path.size())));
	}

	return piecePositionPriorities;
}

void AI::CalcAndPushTurnTilesPriority(std::vector<std::shared_ptr<PositionPriority>>& turnTilesPath, bool startFromOne)
{
	int oldStepCount = 0;
	int priority = 0;

	//При расчете приоритета хода, с обходом других фигур, нужно начинать считать приоритет с единицы
	if (startFromOne)
	{
		priority = 1;
	}

	//Сортируем все возможные ходы фигуры по количеству шагов до базы противника
	std::sort(turnTilesPath.begin(), turnTilesPath.end(), [](const auto& a, const auto& b) {return a->stepCounts < b->stepCounts; });

	//Присваиваем всем возможным ходам фигуры приоритет, в зависимости от количества шагов до базы противника
	for (auto& positionPriority : turnTilesPath)
	{
		if (oldStepCount != positionPriority->stepCounts)
		{
			oldStepCount = positionPriority->stepCounts;
			priority += 2;
		}
		//Записываем приоритет для хода и добавляем его в список возможных ходов
		positionPriority->priority = priority;
		positionPriorities.push_back(positionPriority);
	}
}

void AI::ClearPositionPriorities()
{
	//Очищаем список возможных ходов
	positionPriorities.clear();
}

void AI::SortPositionPriorities()
{
	//Перемешиваем список ходов
	auto seed = unsigned int(std::chrono::system_clock::now().time_since_epoch().count());
	std::shuffle(std::begin(positionPriorities), std::end(positionPriorities), std::default_random_engine(seed));

	//Сортируем все возможные ходы всех фигур по приоритету
	std::sort(positionPriorities.begin(), positionPriorities.end(), [](const auto& a, const auto& b) {return a->priority < b->priority; });
}

std::shared_ptr<PositionPriority> AI::GetBestTurn(Field& field) const
{
	std::shared_ptr<PositionPriority> bestTurn;

	//Определяем лучший ход
	for (auto& positionPriority : positionPriorities)
	{
		//Если на клетке для хода находится фигура, то переходим к проверке следующего хода
		std::shared_ptr<Tile> tile = field.GetTile(positionPriority->position);
		if (tile->piece != nullptr)
		{
			continue;
		}

		//Если фигура была на этой клетке на прошлом ходу, то переходим к проверке следующего хода
		Position oldPosition = positionPriority->piece->GetOldPosition();
		if ((positionPriority->position.x == oldPosition.x) && (positionPriority->position.y == oldPosition.y))
		{
			continue;
		}

		//Записываем текущий ход как лучший
		bestTurn = positionPriority;
		break;
	}

	return bestTurn;
}

std::shared_ptr<PositionPriority> AI::GetPieceNearFinishTiles(Field& field) const
{
	std::shared_ptr<PositionPriority> bestTurn;

	for (auto& piece : pieces)
	{
		//Если фигура находится на базе противника, то переходим к следующей фигуре
		std::shared_ptr<Tile> pieceTile = field.GetTile(piece->GetPosition());
		if (field.СheckEnemyBaseTile(pieceTile, playerNum))
		{
			continue;
		}

		//Ищем клетку базы противника рядом с фигурой
		std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), false);
		for (auto& turnTile : turnTiles)
		{
			if (field.СheckEnemyBaseTile(turnTile, playerNum))
			{
				bestTurn = std::make_shared<PositionPriority>(piece, turnTile->position);
				break;
			}
		}
	}

	return bestTurn;
}

std::shared_ptr<PositionPriority> AI::GetPieceToMoveOnFinishTiles(Field& field) const
{
	std::shared_ptr<PositionPriority> bestTurn;

	for (auto& piece : pieces)
	{
		//Если фигура находится не на базе противника то переходим к следующей фигуре
		std::shared_ptr<Tile> tile = field.GetTile(piece->GetPosition());
		if (!field.СheckEnemyBaseTile(tile, playerNum))
		{
			continue;
		}

		//Сохраняем приоритет клетки под фигурой
		int currentRespawnPriority = tile->respawnPriority;

		std::vector<std::shared_ptr<Tile>> turnTiles = field.GetTilesForTurn(piece->GetPosition(), true);
		//Сравниваем приоритеты соседних клеток с той на которой находится фигура
		for (auto& turnTile : turnTiles)
		{
			//Если соседняя клетка не пустая переходим к следующей клетке 
			if (turnTile->piece != nullptr)
			{
				continue;
			}

			//Если приоритет клетки выше чем тот что у клетки под фигурой запоминаем фигуру и позицию клетки для хода
			if (turnTile->respawnPriority > currentRespawnPriority)
			{
				bestTurn = std::make_shared<PositionPriority>(piece, turnTile->position);
				continue;
			}
		}
	}

	return bestTurn;
}

std::vector<Position> AI::GetBestPath(Position position, Field& field, bool ignorePieces) const
{
	std::vector<Position> bestPath;
	std::vector<std::shared_ptr<Tile>> finishTiles = field.GetFinishTiles(playerNum);
	std::sort(finishTiles.begin(), finishTiles.end(), [](const auto& a, const auto& b) {return a->respawnPriority < b->respawnPriority; });

	size_t stepCount = size_t (std::numeric_limits<int>::max());
	//Расчитываем длину пути до каждой клетки базы противника и выбираем ближайший путь
	for (auto& tile : finishTiles)
	{
		//Если на клетке стоит своя фигура, переходим к следующей клетке
		if ((tile->piece != nullptr) && (tile->piece->GetPlayerNum() == playerNum))
		{
			continue;
		}

		std::vector<Position> path = GetPath(position, tile->position, field, ignorePieces);
		//Если путь до клетки не найден, переходим к следующей клетке
		if (path.size() == 0)
		{
			continue;
		}

		//Если путь до клетки не короче чем у предидущих клеток, переходим к следующей клетке
		if (path.size() > stepCount)
		{
			continue;
		}

		//Записываем путь как наилучший
		stepCount = path.size();
		bestPath = path;
	}

	return bestPath;
}

std::vector<Position> AI::GetPath(Position startPosition, Position endPosition, Field& field, bool ignorePieces) const
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

	//Если путь найден, записываем его
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

	return path;
}

AI::~AI()
{

}
