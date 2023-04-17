#ifndef HANDINFO_H
#define HANDINFO_H

#include <QWidget>

namespace Ui {
class HandInfo;
}

class Hand;

class HandInfo : public QWidget
{
    Q_OBJECT

public:
    explicit HandInfo(Hand &hand);
    ~HandInfo();

    void handUpdated();
    void handIsUpdating();

private:
    Ui::HandInfo *ui;
    Hand &hand;
};

#endif // HANDINFO_H
