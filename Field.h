#pragma once
#include <vector>
#include <memory>

enum class Shade;
struct Position;
struct Tile;
class Piece;
enum class Direction;

class Field
{
public:
	Field(int width, int height, int tileSize);
	void Clear();
	std::shared_ptr<Tile> GetTile(Position position) const;
	std::shared_ptr<Tile> GetTile(int x, int y) const;
	bool SetRespawn(Position position, int playerNum);
	void SetRespawnRect(int x, int y, int width, int height, int playerNum);
	void CalcRespownWeights(int playerNum);
	const std::vector<std::shared_ptr<Tile>> GetRespawn(int playerNum) const;
	const std::vector<std::shared_ptr<Tile>> GetFinishTiles(int playerNum) const;
	void MovePiece(Position position, std::shared_ptr<Piece> piece);
	const std::vector<std::shared_ptr<Tile>> GetTilesForTurn(Position tilePosition, bool ignorePieces) const;
	int GetRespawnWeight(Position position) const;
	int GetWidth() const;
	int GetHeight() const;
	int GetTileSize() const;
	~Field();
private:
	void SetRespawnWeight(Position position, int weight);

	int width = 0;
	int height = 0;
	int tileSize = 0;

	std::vector<std::vector<std::shared_ptr<Tile>>> tiles;
};

struct Position
{
	int x = 0;
	int y = 0;
};

struct Tile
{
	Shade shade;
	Position position{ 0, 0 };
	int respawnPlayerNum = 0;
	int respawnWeight = 0;
	bool availableForTurn = false;
	std::shared_ptr<Piece> piece = nullptr;
};

enum class Shade
{
	LIGHT, DARK
};