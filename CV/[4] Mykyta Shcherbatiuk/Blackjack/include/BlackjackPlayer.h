#ifndef BLACKJACKPLAYER_H 
#define BLACKJACKPLAYER_H 

#include <QPixmap>
#include <vector>

class BlackjackPlayer {
public:
    virtual ~BlackjackPlayer() = default;
    BlackjackPlayer();

    void reset();
    void dealCard(std::pair<QPixmap*, uint8_t> &card_to_value);

    int getHandScore()      { return _hand_score; }
    int getHandCardsCount() { return _hand.size(); }

    auto &getHandCards() { return _hand; }

    bool standing() { return _stand; }
    void stand()    { _stand = true; }

private:
    bool _stand;
    int _hand_score;

    std::vector <std::pair<QPixmap*, uint8_t>> _hand;
};

#endif // BLACKJACKPLAYER_H 