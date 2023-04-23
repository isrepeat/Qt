#include "Button.h"

Button::Button(const char *name, QGraphicsItem *parent) : QGraphicsRectItem(parent) {
    setRect(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
    initColor(Qt::white);
    initText(name);
    setAcceptHoverEvents(true);
}

void Button::initColor(Qt::GlobalColor color) {
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    setBrush(brush);
}

void Button::initText(const char *button_text) {
    _text = new QGraphicsTextItem(button_text, this);
    int x = rect().width() / 2 - _text->boundingRect().width() / 2;
    int y = rect().height() / 2 - _text->boundingRect().height() / 2;
    _text->setPos(x, y);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(QCursor(Qt::ArrowCursor));
    Sound::playClick();
    emit clicked();
}

void Button::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    initColor(Qt::yellow);
    setCursor(QCursor(Qt::PointingHandCursor));
}

void Button::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    initColor(Qt::white);
    setCursor(QCursor(Qt::ArrowCursor));
}
