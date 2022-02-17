#pragma once
#include "Player.h"

struct PositionPriority
{
    std::shared_ptr<Piece> piece = nullptr;
    Position position{ 0, 0 };
    int stepCounts = 0;
    int priority = 0;
    Position positionToMove{ 0, 0 };
};

class AI :
    public Player
{
public:
    AI(int playerNum, Shade shade);
    void Prepare(Field& field) override;
    void Turn(View& view, Controller& controller, Field& field) override;
    ~AI();
private:
    std::vector<std::shared_ptr<PositionPriority>> GetTurnTilesPath(Field& field, std::shared_ptr<Piece> piece, bool ignorePieces);
    void CalcAndPushTurnTilesPriority(std::vector<std::shared_ptr<PositionPriority>>& turnTilesPath, bool startFromOne);
    void ClearPositionPriorities();
    void SortPositionPriorities();
    std::shared_ptr<PositionPriority> GetBestTurn(Field& field) const;
    std::shared_ptr<PositionPriority> GetPieceNearFinishTiles(Field& field) const;
    std::shared_ptr<PositionPriority> GetPieceToMoveOnFinishTiles(Field& field) const;
    std::vector<Position> GetBestPath(Position position, Field& field, bool ignorePieces) const;
    std::vector<Position> GetPath(Position startPosition, Position endPosition, Field& field, bool ignorePieces) const;

    std::vector<std::shared_ptr<PositionPriority>> positionPriorities;
};
