#pragma once
#include "Player.h"

class Human :
    public Player
{
public:
    Human(int playerNum, Shade shade);
    void Prepare(Field& field) override;
    void Turn(View& view, Controller& controller, Field& field) override;
    ~Human();
private:
    bool mouseClick = false;
    bool mouseIsDown = false;
    std::shared_ptr<Piece> pieceSelected = nullptr;
};

