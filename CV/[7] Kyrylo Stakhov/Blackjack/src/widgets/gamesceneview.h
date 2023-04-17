#ifndef GAMESCENEVIEW_H
#define GAMESCENEVIEW_H

#include <QGraphicsView>

class GameSceneView : public QGraphicsView
{
private:
    Q_OBJECT

public:
    GameSceneView(QWidget *parent = nullptr);

    void drawBackground(QPainter *painter, const QRectF &rect) override;
};

#endif // GAMESCENEVIEW_H
