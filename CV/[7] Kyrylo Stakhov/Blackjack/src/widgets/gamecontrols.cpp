#include "widgets/gamecontrols.h"
#include "ui_gamecontrols.h"

GameControls::GameControls(BJWindow &game) :
    QWidget(),
    ui(new Ui::GameControls),
    game(game)
{
    ui->setupUi(this);
}

GameControls::~GameControls()
{
    delete ui;
}

void GameControls::hit()
{
    game.doHit();
}

void GameControls::stand()
{
    game.doStand();
}
