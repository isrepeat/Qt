#include "widgets/handinfo.h"
#include "ui_handinfo.h"

#include "blackjack/hand.h"
#include "resources.h"

HandInfo::HandInfo(Hand &hand) :
    QWidget(),
    ui(new Ui::HandInfo),
    hand(hand)
{
    ui->setupUi(this);
}

HandInfo::~HandInfo()
{
    delete ui;
}

void HandInfo::handUpdated()
{
    ui->infoLabel->setText(Resources::handString.arg(
        hand.lastCard().isFaceVisible() ? hand.evaluate() : hand.firstCard().getScore()
    ));
}

void HandInfo::handIsUpdating()
{
    ui->infoLabel->setText("Dealing cards...");
}
