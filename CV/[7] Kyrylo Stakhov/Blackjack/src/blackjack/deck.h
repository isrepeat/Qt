#ifndef DECK_H
#define DECK_H

#include "blackjack/card.h"

#include <QList>
#include <random>
#include <memory>

class Deck
{
private:
    std::mt19937_64 rng{std::random_device{}()};
    QList<std::shared_ptr<Card>> cards;

public:
    Deck(size_t n = 1);

    /* Populate this deck with `n` * 54 cards */
    void populate(size_t n = 1);

    /* Shuffle all cards in the deck */
    void shuffle();

    /* Get a card from the top of the deck. */
    std::shared_ptr<Card> nextCard();

    /* Get this deck's size */
    size_t size();

    /* Clear this deck */
    void clear();

    /* Clear this deck, populate and shuffle it */
    void reset(size_t n = 1);
};

#endif // DECK_H
