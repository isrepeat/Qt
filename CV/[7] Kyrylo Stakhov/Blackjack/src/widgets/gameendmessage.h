#ifndef GAMEENDMESSAGE_H
#define GAMEENDMESSAGE_H

#include "gameui/bjwindow.h"

#include <QWidget>

namespace Ui {
class GameEndMessage;
}

class GameEndMessage : public QWidget
{
    Q_OBJECT

public:
    explicit GameEndMessage(BJWindow&);
    ~GameEndMessage();

    void setVisible(bool visible) override;

protected:
    void mousePressEvent(QMouseEvent*) override;

private:
    Ui::GameEndMessage *ui;
    BJWindow &game;

    void setTitle(const QString&);
    void setDescription(const QString&);
};

#endif // GAMEENDMESSAGE_H
