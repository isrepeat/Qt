#include "graphics/scene.h"

Scene::Scene() {}

void Scene::centerItem(QGraphicsItem *item)
{
    item->setX(-item->boundingRect().size().width() / 2);
}

Scene& Scene::append(QGraphicsItem *item)
{
    auto dim = item->boundingRect().size();
    item->setY(lastY);
    centerItem(item);
    lastY += dim.height();
    addItem(item);
    return *this;
}

Scene& Scene::prepend(QGraphicsItem *item)
{
    auto offset = item->boundingRect().size().height();
    for(auto sceneItem : items())
        sceneItem->moveBy(0, offset);
    append(item);
    item->setY(0);
    return *this;
}

Scene& Scene::add(QGraphicsItem *item)
{
    item->setY(lastY);
    addItem(item);
    centerItem(item);
    return *this;
}

Scene& Scene::pad(double padding)
{
    lastY += padding;
    return *this;
}

void Scene::center()
{
    for (auto item : items())
        centerItem(item);
}

