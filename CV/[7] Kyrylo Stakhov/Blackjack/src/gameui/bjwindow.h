#ifndef BJWINDOW_H
#define BJWINDOW_H

#include "blackjack/blackjack.h"

#include "graphics/scene.h"
#include "gameconfig.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class BJWindow; }
QT_END_NAMESPACE

class BetControls;
class GameControls;
class GameInfo;
class HandInfo;
class GameEndMessage;

class BJWindow : public QMainWindow
{
    Q_OBJECT

public:
    BJWindow(QWidget *parent = nullptr);
    ~BJWindow();

    const QString& notify(const QString&);

    void resizeEvent(QResizeEvent*) override;
    void closeEvent(QCloseEvent*) override;

    void changeCardSkin(const QString&);

    void doStartGame(int bet);
    void gameStarted();
    void doHit();
    void doStand();
    void gameEnded();

    BlackJack &getBlackJack();

private:
    Ui::BJWindow *ui;
    Scene *scene;

    GameConfig config;

    std::unique_ptr<BlackJack> blackJack;
    BetControls *betControls;
    GameControls *gameControls;
    GameInfo *gameInfo;
    HandInfo *dealerHandInfo, *playerHandInfo;

    static constexpr double LABEL_PADDING = 30;
    static constexpr int
        GAME_END_FADE_IN = 200,
        GAME_END_DURATION = GAME_END_FADE_IN + 5000,
        GAME_END_FADE_OUT = 1000;

    void loadSettings();
    void saveSettings();

    void showGameControls(bool show = true);
    void showBetControls();
    void enableControls(bool value = true);
    void disableControls();

    void resetCards();

    void gameSceneResized();

private slots:
    void doChangeCardSkin();
    void doResetSkin();
    void doResetGame();

    void doSetMusicEnabled(bool enabled);
    void doSetSoundEffectsEnabled(bool enabled);
};

#endif // BJWINDOW_H
