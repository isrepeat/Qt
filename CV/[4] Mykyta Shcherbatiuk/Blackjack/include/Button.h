#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QPointer>
#include <QBrush>
#include <QCursor>

#include "Sound.h"

#define BUTTON_WIDTH  200
#define BUTTON_HEIGHT 50

class Button : public QObject,
               public QGraphicsRectItem {
    Q_OBJECT

signals:
    void clicked();

public:
    Button(const char *name, QGraphicsItem *parent = nullptr);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QPointer <QGraphicsTextItem> _text;

    void initColor(Qt::GlobalColor color);
    void initText(const char *button_text);
};

#endif // BUTTON_H