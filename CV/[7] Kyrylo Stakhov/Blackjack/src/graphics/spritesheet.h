#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>

class SpriteSheet
{
private:
    size_t hRegions, vRegions;
    double regionWidth, regionHeight;
    QPixmap texture;

public:
    SpriteSheet(const QString &path, size_t hRegions, size_t vRegions = 1);

    void paintRegion(
        QPainter *painter,
        QPointF screenCoords,
        size_t regionX, size_t regionY = 1
    );
    QRectF boundingRect() const;

    double getRegionWidth();
    double getRegionHeight();
};

#endif // SPRITESHEET_H
