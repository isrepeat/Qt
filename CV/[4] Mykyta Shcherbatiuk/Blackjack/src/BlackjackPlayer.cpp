#include "BlackjackPlayer.h"

BlackjackPlayer::BlackjackPlayer() {
    reset();
}

void BlackjackPlayer::reset() {
    _stand = false;
    _hand_score = 0;
    _hand.clear();
}

void BlackjackPlayer::dealCard(std::pair<QPixmap*, uint8_t> &card_to_value) {
    _hand.push_back(card_to_value);
    _hand_score += card_to_value.second;
}
