#include "widgets/gamesceneview.h"
#include "resources.h"

GameSceneView::GameSceneView(QWidget *parent)
    : QGraphicsView(parent)
{

}

void GameSceneView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
    auto bgRect = Resources::gameBackground->rect();
    painter->save();
    painter->drawPixmap(
        QRect{-bgRect.width() / 2, 0, bgRect.width(), bgRect.height()},
        *Resources::gameBackground
    );
    painter->restore();
}

