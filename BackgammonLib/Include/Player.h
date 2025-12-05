#pragma once
#include "Color.h"

class Player
{
private:
	Color playerColor;

public:
	Player(Color color);
	~Player();
	Color getColor() const;
};

