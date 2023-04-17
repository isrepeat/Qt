#ifndef GAMECONTROLS_H
#define GAMECONTROLS_H

#include "gameui/bjwindow.h"

#include <QWidget>

namespace Ui {
class GameControls;
}

class GameControls : public QWidget
{
    Q_OBJECT

public:
    explicit GameControls(BJWindow&);
    ~GameControls();

private slots:
    void hit();
    void stand();

private:
    Ui::GameControls *ui;
    BJWindow &game;
};

#endif // GAMECONTROLS_H
