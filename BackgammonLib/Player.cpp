#include "Player.h"

Player::Player(Color color) : playerColor(color) {
}

Player::~Player() {
}

Color Player::GetColor() const {
    return playerColor;
}