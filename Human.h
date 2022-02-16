#pragma once
#include "Player.h"

class Human :
    public Player
{
public:
    Human(int playerNum, Shade shade);
    void Prepare(Field& field) override;
    void Turn(const View& view, Controller& controller, Field& field) override;
    ~Human();
private:
    bool mouseClick = false;
    bool mouseIsDown = false;
    bool isTurn = false;
    std::vector<std::shared_ptr<Tile>> turnTales;
};

