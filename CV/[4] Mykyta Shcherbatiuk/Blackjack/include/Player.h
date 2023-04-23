#ifndef PLAYER_H
#define PLAYER_H

#include <QPixmap>
#include <vector>

#include "BlackjackPlayer.h"

class Player : public BlackjackPlayer {
public:
    Player();
    ~Player() override {}

    int getBalance();
    void setBalance(int balance);

private:
    int _balance;
};

#endif // PLAYER_H