#include "Chip.h"

Chip::Chip(int value, QGraphicsItem *parent) : QGraphicsEllipseItem(parent) {
    _text = nullptr;
    _value = value;
    _svalue = std::to_string(_value);
    setRect(0, 0, CHIP_DIAMETER, CHIP_DIAMETER);
    initColor();
    initText();
    setAcceptHoverEvents(true);
}

Chip::~Chip() {
    delete _text;
}

void Chip::initColor() {
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    switch (_value) {
    case 1:   _color = Qt::gray;        break;
    case 5:   _color = Qt::darkRed;     break;
    case 25:  _color = Qt::darkGreen;   break;
    case 50:  _color = Qt::darkBlue;    break;
    case 100: _color = Qt::black;       break;
    case 500: _color = Qt::darkMagenta; break;
    default:
        throw std::exception("Bad chip value");
    }

    brush.setColor(_color);
    setBrush(brush);
}

void Chip::initText() {
    if (_text != nullptr) {
        delete _text;
    }
    _text = new QGraphicsTextItem(_svalue.c_str(), this);
    _text->setDefaultTextColor(Qt::white);
    int x = rect().width() / 2 - _text->boundingRect().width() / 2;
    int y = rect().height() / 2 - _text->boundingRect().height() / 2;
    _text->setPos(x, y);
}

void Chip::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(QCursor(Qt::ArrowCursor));

    if (event->button() == Qt::LeftButton) {
        emit clicked(true);
    } else if (event->button() == Qt::RightButton) {
        emit clicked(false);
    }
}

void Chip::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    setRect(0, 0, CHIP_HOVER_DIAMETER, CHIP_HOVER_DIAMETER);
    setPos(this->x() - (CHIP_HOVER_DIAMETER - CHIP_DIAMETER) / 2, 
           this->y() - (CHIP_HOVER_DIAMETER - CHIP_DIAMETER) / 2);
    initText();
    setCursor(QCursor(Qt::PointingHandCursor));
}

void Chip::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    setRect(0, 0, CHIP_DIAMETER, CHIP_DIAMETER);
    setPos(this->x() + (CHIP_HOVER_DIAMETER - CHIP_DIAMETER) / 2, 
           this->y() + (CHIP_HOVER_DIAMETER - CHIP_DIAMETER) / 2);
    initText();
    setCursor(QCursor(Qt::ArrowCursor));
}