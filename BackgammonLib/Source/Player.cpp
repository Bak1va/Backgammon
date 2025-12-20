/**
 * @file Player.cpp
 * @brief Implementation of the Player class.
 */

#include "Player.hpp"

Player::Player(Color color) : playerColor(color) {
}

Player::~Player() {
}

Color Player::getColor() const {
    return playerColor;
}