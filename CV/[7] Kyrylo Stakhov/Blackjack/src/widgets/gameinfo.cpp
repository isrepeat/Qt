#include "widgets/gameinfo.h"
#include "ui_gameinfo.h"

#include "resources.h"

GameInfo::GameInfo(BJWindow &game) :
    QWidget(),
    ui(new Ui::GameInfo),
    game(game)
{
    ui->setupUi(this);
}

GameInfo::~GameInfo()
{
    delete ui;
}

void GameInfo::infoUpdated()
{
    auto &blackJack = game.getBlackJack();
    ui->balanceLabel->setText(Resources::balanceString.arg(blackJack.getBalance()));
    ui->betLabel->setText(Resources::betString.arg(blackJack.getBet()));
}
