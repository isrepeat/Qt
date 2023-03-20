#include "Dealer.h"

Dealer::Dealer() {
    _cards_to_values.reserve(53);
    _deck.reserve(52);
    loadCards();
    loadDeck();
}

Dealer::~Dealer() {
    for (auto &card : _cards_to_values) {
        delete card.first;
    }
}

void Dealer::loadCards() {
    loadCardsHelper(UNKNOWN_CARD, 0);

    for (const auto &suit : {"clubs", "diamonds", "hearts", "spades"}) {
        for (int cardnum = 2; cardnum <= 14; ++cardnum) {
            std::stringstream ss;
            ss << ":/card/" << suit << "/" << cardnum;
            loadCardsHelper(ss.str().c_str(), cardnum);
        }
    }
}

void Dealer::loadCardsHelper(const char *cardfile, int cardnum) {
    auto card = new QPixmap(QPixmap(cardfile).scaled(CARD_WIDTH, CARD_HEIGHT));
    int value = 0;

    if       (cardnum <= 10) { value = cardnum; } // { 2, 3, .. 10 } & { 0 }
    else if  (cardnum < 14)  { value = 10; }      // { J, Q, K }
    else                     { value = 11; }      // { A }

    _cards_to_values.push_back(std::make_pair(card, value));
}

void Dealer::loadDeck() {
    _deck.clear();

    for (int i = 0; i < _deck.capacity(); ++i) {
        _deck.push_back(_cards_to_values[i+1]); // skip unknown card
    }

    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    
    std::shuffle(_deck.begin(), _deck.end(), rng);
}

const std::pair<QPixmap*, uint8_t> &Dealer::getCard() {
    if (_deck.size() == _deck.capacity() / 2) {
        loadDeck();
    }
    auto card = _deck.back();
    _deck.pop_back();
    return card;
}
