#include "blackjack/hand.h"
#include "resources.h"
#include "blackjack/blackjack.h"
#include "graphics/scene.h"

#include <QPropertyAnimation>
#include <QApplication>
#include <QGraphicsScene>
#include <QTimer>

std::shared_ptr<Card> Hand::placeholderCard;

Hand::Hand()
{
    if (!placeholderCard)
        placeholderCard = std::make_shared<Card>();

    prepareToDraw();
}

void Hand::add(std::shared_ptr<Card> card)
{
    cards.append(card);
    update();
}

void Hand::centerScene()
{
    auto scene = dynamic_cast<Scene*>(this->scene());
    if (scene)
        scene->center();
}

void Hand::draw(Deck &deck, bool faceVisible)
{
    auto card = deck.nextCard();
    card->setFaceVisible(faceVisible);
    card->setVisible(false);
    add(card);

    /* The scene is pre-centered when dealing the initial 2 cards,
     * so no need to do this here */
    if (cards.size() > 2)
        centerScene();

    doLater([this, card] {
        Resources::audio->playDealSound();
        card->playDrawAnimation(scene());
        --cardsDrawing;
    }, DRAW_DELAY * (cardsDrawing++));
}

void Hand::draw(Deck &deck, Action afterDraw, bool faceVisible)
{
    draw(deck, faceVisible);
    doLater(afterDraw, Card::DRAW_DURATION + DRAW_DELAY * (cardsDrawing - 1));
}

Card &Hand::lastCard()
{
    return *cards.last();
}

Card &Hand::firstCard()
{
    return *cards.first();
}

size_t Hand::evaluate()
{
    size_t score = 0;
    size_t aces = 0;
    for (auto card : cards) {
        auto worth = card->getScore();
        if (card->getRank() == Card::Rank::ACE) {
            ++aces;
            continue;
        }
        score += worth;
    }
    for (size_t i = 0; i < aces; ++i)
        score += score + 11 > BlackJack::BJ ? 1 : 11;
    return score;
}

void Hand::clear()
{
    cards.clear();
}

void Hand::prepareToDraw()
{
    cards.clear();
    cards.append(placeholderCard);
    cards.append(placeholderCard);
    update();
    centerScene();
}

void Hand::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto x = 0;
    for (auto card : cards) {
        card->setX(x);
        x += Resources::cardSheet->getRegionWidth();

        if (card->isVisible())
            card->paint(painter, option, widget);
    }
}

QRectF Hand::boundingRect() const
{
    double width = cards.size() * Resources::cardSheet->getRegionWidth();
    double height = Resources::cardSheet->getRegionHeight();
    return {0, 0, width, height};
}

