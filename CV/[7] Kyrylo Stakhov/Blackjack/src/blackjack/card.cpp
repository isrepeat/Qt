#include "blackjack/card.h"

#include "resources.h"
#include "util/qutil.h"

#include <QtMath>
#include <QGraphicsScene>

Card::Card()
    : Card(Card::Suit::CLUBS, Card::Rank::ACE)
{
    setFaceVisible(false);
}

Card::Card(Suit suit, Rank value)
    : suit(suit), rank(value) {}

Card::Card(size_t suitIdx, size_t rankIdx)
    : Card(toSuit(suitIdx), toRank(rankIdx)) {}

void Card::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto rank_idx = faceVisible ? toIdx(rank) : 0u;
    auto suit_idx = toIdx(faceVisible ? suit : Suit::HIDDEN);
    painter->setOpacity(opacity());
    Resources::cardSheet->paintRegion(
        painter,
        pos(),
        rank_idx,
        suit_idx
    );
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

QRectF Card::boundingRect() const
{
    return Resources::cardSheet->boundingRect();
}

size_t Card::getScore()
{
    if (rank == Rank::ACE)
        return 11u;

    return qMin(10u, toIdx(rank) + 2);
}

Card::Rank Card::getRank()
{
    return rank;
}

Card::Suit Card::getSuit()
{
    return suit;
}

bool Card::isFaceVisible()
{
    return faceVisible;
}

void Card::setFaceVisible(bool value)
{
    faceVisible = value;
}

void Card::hideFace()
{
    setFaceVisible(false);
}

void Card::showFace()
{
    setFaceVisible(true);
}

void Card::playDrawAnimation(QGraphicsScene *scene)
{
    auto pos = this->pos();
    QPointF startAt(pos.x(), (scene->itemsBoundingRect().height() - pos.y()) / 2);
    auto moveAnim = makePropertyAnimation(this, "pos", scene);
    moveAnim->setDuration(DRAW_DURATION);
    moveAnim->setStartValue(startAt);
    moveAnim->setEndValue(pos);
    moveAnim->setEasingCurve({QEasingCurve::InBack});
    moveAnim->start(QAbstractAnimation::DeleteWhenStopped);
    fadeIn(this, scene, 300);
    setVisible(true);
}
