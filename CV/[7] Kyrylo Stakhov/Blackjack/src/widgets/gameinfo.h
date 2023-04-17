#ifndef GAMEINFO_H
#define GAMEINFO_H

#include "gameui/bjwindow.h"

#include <QWidget>

namespace Ui {
class GameInfo;
}

class BlackJack;

class GameInfo : public QWidget
{
    Q_OBJECT

public:
    explicit GameInfo(BJWindow&);
    ~GameInfo();

    void infoUpdated();

private:
    Ui::GameInfo *ui;
    BJWindow &game;
};

#endif // GAMEINFO_H
