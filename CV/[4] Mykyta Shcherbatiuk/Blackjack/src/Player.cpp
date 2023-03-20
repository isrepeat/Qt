#include "Player.h"

Player::Player() {
    _balance    = 1000;
}

int Player::getBalance() {
    return _balance;
}

void Player::setBalance(int balance) {
    _balance = balance;
}
