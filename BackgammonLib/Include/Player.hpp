#pragma once
#include "Color.hpp"

class Player
{
private:
	Color playerColor;

public:
	Player(Color color);
	~Player();
	Color getColor() const;
};

