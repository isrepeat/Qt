#ifndef BETCONTROLS_H
#define BETCONTROLS_H

#include "gameui/bjwindow.h"

#include <QWidget>

namespace Ui {
class BetControls;
}

class BetControls : public QWidget
{
    Q_OBJECT

public:
    explicit BetControls(BJWindow&);
    ~BetControls();

    void balanceUpdated();

private slots:
    void placeBet();

private:
    Ui::BetControls *ui;
    BJWindow &game;
};

#endif // BETCONTROLS_H
