#include "blackjack/deck.h"

#include <algorithm>

Deck::Deck(size_t n)
{
    populate(n);
    shuffle();
}

void Deck::populate(size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        for (size_t suit = 0; suit < Card::SUITS; ++suit) {
            for (size_t rank = 0; rank < Card::RANKS; ++rank) {
                cards.append(std::make_shared<Card>(suit, rank));
            }
        }
    }
}

void Deck::shuffle()
{
    std::shuffle(cards.begin(), cards.end(), rng);
}

std::shared_ptr<Card> Deck::nextCard()
{
    auto card = cards.last();
    cards.removeLast();
    return card;
}

size_t Deck::size()
{
    return cards.size();
}

void Deck::clear()
{
    cards.clear();
}

void Deck::reset(size_t n)
{
    clear();
    populate(n);
    shuffle();
}

