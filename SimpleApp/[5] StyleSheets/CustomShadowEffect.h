#pragma once
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <mutex>

class CustomShadowEffect : public QGraphicsEffect {
    Q_OBJECT

public:
    explicit CustomShadowEffect(QObject* parent = 0);

    void draw(QPainter* painter);
    QRectF boundingRectFor(const QRectF& rect) const override;

    void setBorder(qreal border) { 
        //std::unique_lock<std::mutex> lk(mx);
        _border = border; 
        updateBoundingRect(); 
    }
    qreal border() const {
        //std::unique_lock<std::mutex> lk(mx);
        return _border; 
    }

    void setBlurRadius(qreal blurRadius) { 
        //std::unique_lock<std::mutex> lk(mx);
        _blurRadius = blurRadius;
        updateBoundingRect();
    }
    qreal blurRadius() const {
        //std::unique_lock<std::mutex> lk(mx);
        return _blurRadius;
    }

    void setColor(const QColor& color) {
        //std::unique_lock<std::mutex> lk(mx);
        _color = color;
    }
    QColor color() const {
        //std::unique_lock<std::mutex> lk(mx);
        return _color;
    }

private:

    QColor _color;
    qreal  _border;
    qreal  _blurRadius;

    mutable QRectF origRect;
    mutable std::mutex mx;
};