#include "gameui/bjwindow.h"
#include "ui_bjwindow.h"

#include "resources.h"
#include "util/dialogs.h"

#include "widgets/gamecontrols.h"
#include "widgets/betcontrols.h"
#include "widgets/gameinfo.h"
#include "widgets/handinfo.h"
#include "widgets/gameendmessage.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QScrollBar>
#include <QFileDialog>

BJWindow::BJWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::BJWindow),
    scene(new Scene),
    config(*this),
    blackJack(std::make_unique<BlackJack>())
{
    Resources::load();
    ui->setupUi(this);
    ui->gameView->verticalScrollBar()->blockSignals(true);

    betControls = new BetControls(*this);
    gameControls = new GameControls(*this);
    gameInfo = new GameInfo(*this);
    dealerHandInfo = new HandInfo(blackJack->getDealerHand());
    playerHandInfo = new HandInfo(blackJack->getPlayerHand());
    showBetControls();

    /* Populate the game scene */
    scene->append(scene->addWidget(gameInfo)).pad(LABEL_PADDING);
    scene->append(&blackJack->getDealerHand());
    scene->append(scene->addWidget(dealerHandInfo)).pad(LABEL_PADDING);
    scene->append(&blackJack->getPlayerHand());
    scene->append(scene->addWidget(playerHandInfo)).pad(LABEL_PADDING);
    scene->add(scene->addWidget(betControls));
    scene->append(scene->addWidget(gameControls));
    ui->gameView->setScene(scene);

    show();
    gameSceneResized();
    loadSettings();
}

BJWindow::~BJWindow()
{
    delete ui;
}

void BJWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    gameSceneResized();
}

void BJWindow::closeEvent(QCloseEvent*)
{
    saveSettings();
}

void BJWindow::changeCardSkin(const QString &fileName)
{
    bool loaded = false;
    try {
        QFile sheetFile(fileName);
        if (!fileName.isEmpty() && sheetFile.exists()) {
            Resources::loadCardSheet(fileName);
            loaded = true;
        } else
            Dialog::error("Error", "Couldn't open specified card skin file");
    } catch (...) {
        Dialog::error("Error", "An exception has occurred while trying to load the card skin");
        Resources::resetCardSheet();
    }
    scene->update();

    if (loaded)
        config.customSkinLoaded(fileName);
    else
        config.customSkinUnloaded();
}

void BJWindow::doStartGame(int bet)
{
    resetCards();
    playerHandInfo->handIsUpdating();
    dealerHandInfo->handIsUpdating();
    if (blackJack->startGame(bet, [this]{gameStarted();})) {
        Resources::audio->playBetSound();
        showGameControls();
        disableControls();
    }
    else
        Dialog::error("Error", "Bet amount is invalid!");
}

void BJWindow::gameStarted()
{
    qDebug() << "Game started!";
    enableControls();
    gameInfo->infoUpdated();
    playerHandInfo->handUpdated();
    dealerHandInfo->handUpdated();
    if (!blackJack->isGameInProgress())
        gameEnded();
}

void BJWindow::doHit()
{
    Resources::audio->playClickSound();
    playerHandInfo->handIsUpdating();
    disableControls();
    blackJack->hit([this] {
        playerHandInfo->handUpdated();
        dealerHandInfo->handUpdated();
        enableControls();
        if (!blackJack->isGameInProgress())
            gameEnded();
    });
    scene->center();
    gameSceneResized();
}

void BJWindow::doStand()
{
    Resources::audio->playClickSound();
    notify("Dealer's turn");
    dealerHandInfo->handIsUpdating();
    disableControls();
    blackJack->stand([this] {
        update();
        playerHandInfo->handUpdated();
        dealerHandInfo->handUpdated();
        doLater([this] {
            gameEnded();
        }, 250);
    });
    scene->center();
    gameSceneResized();
}

void BJWindow::gameEnded()
{
    if (blackJack->getBalance() > 0)
        enableControls();

    showBetControls();
    betControls->balanceUpdated();
    gameInfo->infoUpdated();

    auto endMessage = scene->addWidget(new GameEndMessage(*this));
    endMessage->setMinimumSize(ui->gameView->width(), ui->gameView->height());
    scene->center();
    TaskChain()
        .schedule([this, endMessage] {
            fadeIn(endMessage, scene, GAME_END_FADE_IN);
        })
        .schedule([this, endMessage] {
            fadeOut(endMessage, scene, GAME_END_FADE_OUT);
        }, GAME_END_DURATION)
        .schedule([this, endMessage] {
            scene->removeItem(endMessage);
        }, GAME_END_FADE_OUT);
}

BlackJack &BJWindow::getBlackJack()
{
    return *blackJack;
}

void BJWindow::loadSettings()
{
    config.load();
    gameInfo->infoUpdated();
    betControls->balanceUpdated();
    ui->toggleMusicAction->setChecked(Resources::audio->isBgMusicPlaying());
    ui->toggleSoundEffectsAction->setChecked(Resources::audio->areSoundEffectsEnabled());
}

void BJWindow::saveSettings()
{
    config.save();
}

void BJWindow::showGameControls(bool show)
{
    notify(show ? "Your turn." : "Place a bet to begin the game.");
    gameControls->setVisible(show);
    betControls->setVisible(!show);
}

void BJWindow::showBetControls()
{
    showGameControls(false);
}

void BJWindow::enableControls(bool value)
{
    gameControls->setEnabled(value);
    betControls->setEnabled(value);
}

void BJWindow::disableControls()
{
    enableControls(false);
}

void BJWindow::resetCards()
{
    blackJack->getDealerHand().prepareToDraw();
    blackJack->getPlayerHand().prepareToDraw();
}

void BJWindow::gameSceneResized()
{
    auto width = ui->gameView->width();
    auto height = ui->gameView->height();
    auto controlsWidth = width / 5;
    scene->setSceneRect(-width / 2, 0, width, height);
    ui->gameView->fitInView(scene->itemsBoundingRect(), Qt::AspectRatioMode::KeepAspectRatio);
    ui->gameView->centerOn(0, 0);
    betControls->setMinimumWidth(controlsWidth);
    gameControls->setMinimumWidth(controlsWidth);
    scene->center();
}

const QString& BJWindow::notify(const QString &message)
{
    ui->statusbar->showMessage(message);
    return message;
}

void BJWindow::doChangeCardSkin()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        "Open Card Skin",
        "./",
        "Image Files (*.png *.jpg *.bmp)"
    );
    changeCardSkin(fileName);
}

void BJWindow::doResetSkin()
{
    Resources::resetCardSheet();
    scene->update();
    config.customSkinUnloaded();
}

void BJWindow::doResetGame()
{
    if (blackJack->isGameInProgress()) {
        showBetControls();
        resetCards();
    }

    enableControls();
    blackJack->resetBalance();
    gameInfo->infoUpdated();
    betControls->balanceUpdated();
}

void BJWindow::doSetMusicEnabled(bool enabled)
{
    auto &player = Resources::audio->getBgMusicPlayer();
    if (!enabled)
        player.stop();
    else
        player.play();
}

void BJWindow::doSetSoundEffectsEnabled(bool enabled)
{
    Resources::audio->enableSoundEffects(enabled);
}


