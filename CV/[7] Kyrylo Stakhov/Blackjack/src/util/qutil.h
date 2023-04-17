#ifndef QUTIL_H
#define QUTIL_H

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QTimer>
#include <QtMath>

/* Make a property animation for a QGraphicsObject */
QPropertyAnimation* makePropertyAnimation(
    QGraphicsObject *obj, const QByteArray& propName, QGraphicsScene *scene
);

QPropertyAnimation* fade(
    QGraphicsObject *obj, QGraphicsScene *scene,
    int durationMsec, bool fadeIn);

inline QPropertyAnimation* fadeIn(
    QGraphicsObject *obj, QGraphicsScene *scene, int durationMsec
)
{
    return fade(obj, scene, durationMsec, true);
}


inline QPropertyAnimation* fadeOut(
    QGraphicsObject *obj, QGraphicsScene *scene, int durationMsec
)
{
    return fade(obj, scene, durationMsec, false);
}

template<typename F>
void doLater(F &&action, int waitMsec)
{
    QTimer::singleShot(qMax(0, waitMsec), action);
}

/* A chain of tasks that are scheduled to be executed at
 * some points in the future one after another. */
class TaskChain
{
private:
    int delay = 0;

public:
    template<typename F>
    inline TaskChain& schedule(F &&task, int waitMsec = 5)
    {
        doLater(task, delay += waitMsec);
        return *this;
    }
};

#endif // QUTIL_H
