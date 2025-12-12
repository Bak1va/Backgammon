#pragma once

#include <array>
#include "Color.hpp"

struct GameStateDTO {
    std::array<int, 24> pieceCounts{};
    std::array<Color, 24> colors{};

    int barWhite = 0;
    int barBlack = 0;

    int borneOffWhite = 0;
    int borneOffBlack = 0;

    Color currentPlayer = Color::NONE;

    int dice1 = 0;
    int dice2 = 0;
};
