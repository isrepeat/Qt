#include "widgets/gameendmessage.h"
#include "ui_gameendmessage.h"

#include "resources.h"

GameEndMessage::GameEndMessage(BJWindow &game) :
    QWidget(),
    ui(new Ui::GameEndMessage),
    game(game)
{
    ui->setupUi(this);
}

GameEndMessage::~GameEndMessage()
{
    delete ui;
}

void GameEndMessage::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (!visible)
        return;

    auto &blackJack = game.getBlackJack();

    switch(blackJack.getGameState()) {
    case BlackJack::GameState::Bust:
        setTitle("Bust");
        setDescription(Resources::bustMsg.arg(blackJack.getBet()));
        Resources::audio->playLossSound();
        break;

    case BlackJack::GameState::Loss:
        setTitle("Loss");
        setDescription(Resources::lossMsg.arg(blackJack.getBet()));
        Resources::audio->playLossSound();
        break;

    case BlackJack::GameState::Push:
        setTitle("Push");
        setDescription(Resources::pushMsg);
        Resources::audio->playPushSound();
        break;

    case BlackJack::GameState::BlackJack:
        setTitle("BlackJack");
        setDescription(Resources::winMsg.arg(blackJack.winAmount()));
        Resources::audio->playWinSound();
        break;

    case BlackJack::GameState::Win:
        setTitle("Win");
        setDescription(Resources::winMsg.arg(blackJack.winAmount()));
        Resources::audio->playWinSound();
        break;

    default:
        break;
    }
}

void GameEndMessage::mousePressEvent(QMouseEvent*)
{
    hide();
}

void GameEndMessage::setTitle(const QString &title)
{
    ui->gameStateLabel->setText(title);
}

void GameEndMessage::setDescription(const QString &description)
{
    ui->descriptionLabel->setText(description);
}
