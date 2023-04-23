#ifndef HAND_H
#define HAND_H

#include "types.h"
#include "blackjack/card.h"
#include "blackjack/deck.h"
#include "util/qutil.h"

#include <QGraphicsObject>
#include <QList>

class Hand : public QGraphicsObject
{
private:
    Q_OBJECT

    QList<std::shared_ptr<Card>> cards;
    int cardsDrawing = 0;

    void add(std::shared_ptr<Card>);
    void centerScene();

    static std::shared_ptr<Card> placeholderCard;

public:
    static constexpr int DRAW_DELAY = 275;

    Hand();

    /* Draw a card from the deck */
    void draw(Deck&, bool faceVisible = true);

    inline void drawHidden(Deck &deck)
    {
        draw(deck, false);
    }

    /* Draw a card from the deck and execute the `afterDraw` action
     * after card drawing animation is finished */
    void draw(Deck &deck, Action afterDraw, bool faceVisible = true);

    inline void drawHidden(Deck &deck, Action afterDraw)
    {
        draw(deck, afterDraw, false);
    }

    /* Get a reference to the first/last card drawn from the deck */
    Card& lastCard();
    Card& firstCard();

    /* Evaluate this hand's score */
    size_t evaluate();

    /* Remove all cards from this hand */
    void clear();

    /* Calculate time required to play the draw animation of `nCards` cards,
     * which are drawn from the deck sequentially. */
    inline static constexpr int timeToDraw(int nCards)
    {
        int delay = 0;
        for (int cardsDrawing = 0; cardsDrawing < nCards; ++cardsDrawing)
            delay += DRAW_DELAY * cardsDrawing;
        return delay;
    }

    /* Fill this hand with 2 placeholder cards.
     * Used to center the scene before the beginning of a new game. */
    void prepareToDraw();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
};

#endif // HAND_H
