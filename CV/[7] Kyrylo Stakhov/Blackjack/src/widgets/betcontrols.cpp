#include "widgets/betcontrols.h"
#include "ui_betcontrols.h"

BetControls::BetControls(BJWindow &game) :
    QWidget(),
    ui(new Ui::BetControls),
    game(game)
{
    ui->setupUi(this);
    ui->betSlider->setValue(10);
    balanceUpdated();
}

BetControls::~BetControls()
{
    delete ui;
}

void BetControls::balanceUpdated()
{
    auto balance = game.getBlackJack().getBalance();
    ui->betSlider->setMinimum(1);
    ui->betSpinBox->setMinimum(1);
    ui->betSlider->setMaximum(balance);
    ui->betSpinBox->setMaximum(balance);
}

void BetControls::placeBet()
{
    game.doStartGame(ui->betSlider->value());
}
