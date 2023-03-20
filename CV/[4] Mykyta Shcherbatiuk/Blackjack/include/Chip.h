#ifndef CHIP_H
#define CHIP_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QPointer>
#include <QBrush>
#include <QCursor>
#include <string>

#define CHIP_DIAMETER 70
#define CHIP_HOVER_DIAMETER (CHIP_DIAMETER * 1.25)

class Chip : public QObject,
             public QGraphicsEllipseItem {
    Q_OBJECT

signals:
    void clicked(bool add);

public:
    Chip(int value, QGraphicsItem *parent = nullptr);
    ~Chip();

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QPointer <QGraphicsTextItem> _text;
    Qt::GlobalColor              _color;
    int                          _value;
    std::string                  _svalue;

    void initColor();
    void initText();

};

#endif // CHIP_H