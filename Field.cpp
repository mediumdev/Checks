#include "Field.h"
#include "Piece.h"

Field::Field(int width, int height, int tileSize) : width(width), height(height), tileSize(tileSize)
{
    tiles.resize(width);
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            tiles[i].push_back(std::make_unique<Tile>());
            tiles[i][j]->position.x = i;
            tiles[i][j]->position.y = j;
        }
    }
}

void Field::Clear()
{
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            tiles[i][j]->shade = (((i + j) % 2 == 0) ? Shade::LIGHT : Shade::DARK);
            tiles[i][j]->respawnPlayerNum = 0;
            tiles[i][j]->piece = nullptr;
        }
    }
}

std::shared_ptr<Tile> Field::GetTile(Position position) const
{
    return tiles[position.x][position.y];
}

std::shared_ptr<Tile> Field::GetTile(int x, int y) const
{
    return tiles[x][y];
}

bool Field::SetRespawn(Position position, int playerNum)
{
    if ((position.x < 0) || (position.y < 0) || (position.x >= width) || (position.y >= height))
    {
        return false;
    }

    if (tiles[position.x][position.y]->respawnPlayerNum != 0)
    {
        return false;
    }

    std::shared_ptr<Tile> tile = tiles[position.x][position.y];
    tile->respawnPlayerNum = playerNum;

    return true;
}

void Field::SetRespawnRect(int x, int y, int width, int height, int playerNum)
{
    for (int i = x; i < x + width; i++)
    {
        for (int j = y; j < y + height; j++)
        {
            Position position;
            position.x = i;
            position.y = j;
            SetRespawn(position, playerNum);
        }
    }

    CalcRespownPriorities(playerNum);
}

void Field::CalcRespownPriorities(int playerNum)
{
    std::vector<std::shared_ptr<Tile>> respawnTiles = GetRespawn(playerNum);
    for (auto& tile : respawnTiles)
    {
        std::vector<std::shared_ptr<Tile>> turnTiles = GetTilesForTurn(tile->position, true);
        int priority = 1;
        for (auto& turnTile : turnTiles)
        {
            if (turnTile->respawnPlayerNum == playerNum)
            {
                priority += 9 - turnTiles.size() * 2;
            }
            else
            {
                priority--;
            }
        }

         SetRespawnPriority(tile->position, priority);
    }
}

std::vector<std::shared_ptr<Tile>> Field::GetRespawn(int playerNum) const
{
    std::vector<std::shared_ptr<Tile>> respawn;

    for (auto& i : tiles)
    {
        for (auto& tile : i)
        {
            if (tile->respawnPlayerNum == playerNum)
            {
                respawn.push_back(tile);
            }
        }
    }

    return respawn;
}

std::vector<std::shared_ptr<Tile>> Field::GetFinishTiles(int playerNum) const
{
    std::vector<std::shared_ptr<Tile>> finishTiles;

    for (auto& i : tiles)
    {
        for (auto& tile : i)
        {
            if (tile->respawnPlayerNum == 0)
            {
                continue;
            }

            if (tile->respawnPlayerNum == playerNum)
            {
                continue;
            }

            finishTiles.push_back(tile);
        }
    }

    return finishTiles;
}

void Field::MovePiece(Position position, std::shared_ptr<Piece> piece)
{
    auto oldPosition = piece->GetPosition();
    tiles[oldPosition.x][oldPosition.y]->piece = nullptr;
    tiles[position.x][position.y]->piece = piece;
    tiles[position.x][position.y]->piece->SetPosition(position);
}

std::vector<std::shared_ptr<Tile>> Field::GetTilesForTurn(Position tilePosition, bool ignorePieces) const
{
    std::vector<std::shared_ptr<Tile>> tilesForTurn;

    int direction[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    for (auto& i : direction)
    {
        int x = tilePosition.x + i[0];
        int y = tilePosition.y + i[1];

        if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
        {
            continue;
        }

        std::shared_ptr<Tile> tile = tiles[x][y];

        if ((tile->piece != nullptr) && (!ignorePieces))
        {
            continue;
        }

        tilesForTurn.push_back(tile);
    }

    return tilesForTurn;
}

void Field::SetRespawnPriority(Position position, int priority)
{
    if ((position.x < 0) || (position.y < 0) || (position.x >= width) || (position.y >= height))
    {
        return;
    }

    if (tiles[position.x][position.y]->respawnPlayerNum == 0)
    {
        return;
    }

    tiles[position.x][position.y]->respawnPriority = priority;
}

int Field::GetRespawnPriority(Position position) const
{
    if ((position.x < 0) || (position.y < 0) || (position.x >= width) || (position.y >= height))
    {
        return -1;
    }

    if (tiles[position.x][position.y]->respawnPlayerNum == 0)
    {
        return - 1;
    }

    return tiles[position.x][position.y]->respawnPriority;
}

bool Field::ÑheckEnemyBaseTile(std::shared_ptr<Tile> tile, int playerNum) const
{
    if ((tile->respawnPlayerNum != playerNum) && (tile->respawnPlayerNum != 0))
    {
        return true;
    }

    return false;
}

int Field::GetWidth() const
{
    return width;
}

int Field::GetHeight() const
{
    return height;
}

int Field::GetTileSize() const
{
    return tileSize;
}

Field::~Field()
{

}
