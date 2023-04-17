#include "graphics/spritesheet.h"

#include <QDebug>

SpriteSheet::SpriteSheet(const QString &path, size_t hRegions, size_t vRegions)
    : hRegions(hRegions), vRegions(vRegions),
    texture(path)
{
    regionWidth = texture.width() / hRegions;
    regionHeight = texture.height() / vRegions;
    qDebug() << "New SpriteSheet: " << texture.width() << "x" << texture.height();
    qDebug() << "Regions: " << hRegions << "x" << vRegions;
    qDebug() << "Region dimensions: " << regionWidth << "x" << regionHeight;
}

void SpriteSheet::paintRegion(
    QPainter *painter,
    QPointF screenCoords,
    size_t regionX, size_t regionY
)
{
    painter->drawPixmap(
        screenCoords.x(),
        screenCoords.y(),
        texture,
        regionX * regionWidth, regionY * regionHeight,
        regionWidth, regionHeight
    );
}

QRectF SpriteSheet::boundingRect() const
{
    return {
        -regionWidth / 2,
        -regionHeight / 2,
        regionWidth, regionHeight
    };
}

double SpriteSheet::getRegionWidth()
{
    return regionWidth;
}

double SpriteSheet::getRegionHeight()
{
    return regionHeight;
}
