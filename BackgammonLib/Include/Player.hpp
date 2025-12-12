#pragma once
#include "Color.hpp"

class Player {
public:
    Player(Color color);
    ~Player();

    Color getColor() const;

private:
    Color playerColor;
};
