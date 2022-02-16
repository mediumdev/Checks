#pragma once
#include "Player.h"

class AI :
    public Player
{
public:
    AI(int playerNum, Shade shade);
    void Prepare(Field& field) override;
    void Turn(const View& view, Controller& controller, Field& field) override;
    ~AI();
private:
    void PreparePiecesTurnWeights(Field& field);
    void CalcPiecesTurnWeights(Field& field, std::shared_ptr<Piece> piece, bool ignorePieces);
    std::shared_ptr<Piece> GetPiceNearFinishTiles(Field& field);
    std::shared_ptr<Piece> GetPiceToMoveOnFinishTiles(Field& field) const;
    std::vector<Position> GetBestPath(Position position, Field& field, bool ignorePieces);
    std::vector<Position> GetPath(Position startPosition, Position endPosition, Field& field, bool ignorePieces);
};
