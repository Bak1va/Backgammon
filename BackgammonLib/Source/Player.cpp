#include "Player.hpp"

Player::Player(Color color) : playerColor(color) {
}

Player::~Player() {
}

Color Player::getColor() const {
    return playerColor;
}