#include "util/qutil.h"

QPropertyAnimation* makePropertyAnimation(
    QGraphicsObject *obj, const QByteArray &propName, QGraphicsScene *scene
)
{
    auto anim = new QPropertyAnimation(obj, propName);
    QObject::connect(
        anim, &QPropertyAnimation::valueChanged,
        [scene](auto&) {
            scene->update();
        }
    );
    return anim;
}

QPropertyAnimation *fade(
    QGraphicsObject *obj, QGraphicsScene *scene,
    int durationMsec, bool fadeIn
)
{
    auto fade = makePropertyAnimation(obj, "opacity", scene);
    fade->setStartValue(fadeIn ? 0.0 : 1.0);
    fade->setEndValue(fadeIn ? 1.0 : 0.0);
    fade->setDuration(durationMsec);
    fade->start(QAbstractAnimation::DeleteWhenStopped);
    return fade;
}
