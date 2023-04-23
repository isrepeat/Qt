#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>

class Scene : public QGraphicsScene
{
private:
    double lastY = 0;

    void centerItem(QGraphicsItem*);

public:
    Scene();

    /* Append a QGraphicsItem to the bottom of this scene. */
    Scene& append(QGraphicsItem*);

    /* Insert a QGraphicsItem at the top of this scene */
    Scene& prepend(QGraphicsItem*);

    /* Add a QGraphicsItem without incrementing the scene's vertical offset */
    Scene& add(QGraphicsItem*);

    /* Add padding after the last inserted element */
    Scene& pad(double padding);

    /* Center all items in this scene horizontally */
    void center();

    template<typename T>
    void forEachItem(T &&action)
    {
        for (auto item : items())
            action(item);
    }
};

#endif // SCENE_H
