#if STYLE_SHEETS == 1
#include "CustomShadowEffect.h"
#include <QPainter>

CustomShadowEffect::CustomShadowEffect(QObject* parent) :
    QGraphicsEffect(parent),
    _blurRadius(10.0f),
    _color(0, 0, 0, 80)
{
}

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

void CustomShadowEffect::draw(QPainter* painter)
{
    //// if nothing to show outside the item, just draw source
    //if ((blurRadius() + distance()) <= 0) {
    //    drawSource(painter);
    //    return;
    //}

    PixmapPadMode mode = QGraphicsEffect::PadToEffectiveBoundingRect;
    QPoint offset;
    const QPixmap px = sourcePixmap(Qt::DeviceCoordinates, &offset, mode);

    // return if no source
    if (px.isNull())
        return;

    // save world transform
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());

    auto pxRect = px.rect();
    //auto origRect = QRect(0, 0, px.rect().width() - border() * 2, px.rect().height() - border() * 2);

    auto dpi = px.devicePixelRatioF();

    /*
    * ---------------------------------------------------------
    * |                |                                       |
    * |                | dy                                    |
    * |                |                                       |
    * |      --------------------------------------------      |
    * |     |        ____________________________        |     |
    * |  dx |       |        | m                 |       |     |
    * |-----|       |     ------------------     |       |     | 
    * |     |       |  m  |                 |by' |       |h'   |h
    * |     |       |-----|        bx'      |    |by     |     |
    * |     |       |     ------------------     |       |     |
    * |     |       |____________________________|       |     |
    * |     |                      bx                    |     |
    * |      --------------------------------------------      |
    * |                           w'                           |
    * ---------------------------------------------------------
    *                             w

    * w' = w - 2dx;  dx = w * kx;  bx - bx' = 2m;
    * h' = h - 2dy;  dy = h * ky;  by - by' = 2m;
    * 
    * bx/w = bx'/w';  bx' = bx * w'/w  =>  bx' = bx*(1-kx);
    * by/h = by'/h';  by' = by * h'/h  =>  by' = by*(1-ky);
    * 
    * kx = 2m/bx;
    * ky = 2m/by;
    */

    float m = border();

    int w = px.size().width();
    int h = px.size().height();

    int bx = origRect.width() * 3;
    int by = origRect.height() * 3;

    float kx = 2*m / bx;
    float ky = 2*m / by;

    float dX = (w * kx) / 2;
    float dY = (h * ky) / 2;

    //float dX = -w/2 / 2 /*/ 10*/;
    //float dY = -h/2 / 2;

    //QSize szi(px.size().width(), px.size().height());
    QSize szi(w + 2 * dX, h + 2 * dY);

    QPixmap scaled = px.scaled(szi);
    auto scaledSz = scaled.size();

    QImage tmp(szi, QImage::Format_ARGB32_Premultiplied);
    //tmp.setDevicePixelRatio(px.devicePixelRatioF());
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPointF(0, 0), scaled);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
    //blurred.setDevicePixelRatio(px.devicePixelRatioF());
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmp, blurRadius(), false, true);
    blurPainter.end();

    tmp = blurred;

    // blacken the image...
    tmpPainter.begin(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmp.rect(), color());
    //tmpPainter.fillRect(tmp.rect(), QColor("#ff0000"));
    tmpPainter.end();

    //auto newOX = 500 / 2;
    //auto newOY = 500 / 2;

    auto newOX = -dX / dpi;
    auto newOY = -dY / dpi;

    //auto newOX = 0;
    //auto newOY = 0;

    // draw the blurred shadow...
    //painter->drawImage(offset, tmp);
    painter->drawImage(QPoint(offset.x() + newOX, offset.y() + newOY), tmp);

    // draw the actual pixmap...
    painter->drawPixmap(offset, px, QRectF());

    // restore world transform
    painter->setWorldTransform(restoreTransform);
}

QRectF CustomShadowEffect::boundingRectFor(const QRectF& rect) const {
    origRect = rect;

    auto offset = QPointF(0, 0);
    int margin = border() + blurRadius();
    //int margin = 40;
    //auto translated = rect;
    auto translated = rect.translated(offset);
    //translated = translated.translated(QPointF(50, 50));
    auto adjusted = translated.adjusted(-margin, -margin, margin, margin);
    auto united = rect.united(adjusted);
    return united;
}
#endif