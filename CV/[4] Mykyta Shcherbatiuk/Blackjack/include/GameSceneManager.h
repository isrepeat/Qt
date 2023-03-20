#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemAnimation>
#include <QPropertyAnimation>
#include <QTimeLine>
#include <QTimer>
#include <QPointer>
#include <QPainter>
#include <QPixmap>
#include <QFont>
#include <memory>
#include <vector>
#include <sstream>

#include "BlackjackPlayer.h"
#include "Dealer.h"
#include "Player.h"
#include "Button.h"
#include "Chip.h"
#include "Sound.h"

#define FIRST_CHIP_X       350
#define CHIPS_HEIGHT       200
#define DEAL_TEXT_X        475
#define DEAL_TEXT_Y        100
#define BETTED_TEXT_X      50
#define BETTED_TEXT_Y      600
#define BALANCE_TEXT_X     850
#define BALANCE_TEXT_Y     600
#define FONT_SIZE          32
#define PLAYERCARD_Y       350
#define DEALERCARD_Y       100
#define CARD_X             350
#define PLAYER_SCORE_Y     400
#define DEALER_SCORE_Y     150
#define SCORE_X            35
#define BUTTON_HIT_Y       325
#define BUTTON_STAND_Y     250
#define BUTTON_STAND_HIT_X 75
#define GAMERESULT_X       600
#define GAMERESULT_Y       325

class GameSceneManager : public QObject {
    Q_OBJECT

public:
    GameSceneManager(QGraphicsScene *scene, int resolution_x, int resolution_y);
    ~GameSceneManager();

    void start();
    void clearDealScene();

private:
    QGraphicsScene          *_scene;
    std::vector <Chip*>      _chips;
    std::unique_ptr <Dealer> _dealer;
    std::unique_ptr <Player> _player;
    QPointer <Button>        _button_deal;
    QPointer <Button>        _button_hit;
    QPointer <Button>        _button_stand;
    int                      _resolution_x;
    int                      _resolution_y;
    int                      _bet_sum;

    std::unique_ptr <QFont>  _font;
    QGraphicsTextItem       *_deal_text;
    QGraphicsTextItem       *_balance_text;
    QGraphicsTextItem       *_playerscore_text;
    QGraphicsTextItem       *_dealerscore_text;
    QGraphicsTextItem       *_gameresult_text;
    QTimer                  *_timer;


    std::vector <QGraphicsPixmapItem*> _pixmap_items;

    void addPlayerBalance();
    void addDealSceneItems();
    void initDealSceneButtons();
    void initDealSceneChips();

    void initHitStandScene();
    void initHitStandSceneButtons();
    void addHitStandSceneItems();

    void initButton(QPointer<Button> &button,
                    const char *button_text,
                    int button_y, int button_x = -1);

    void setMessage(QGraphicsTextItem **textitem, 
                    const char *textmessage,
                    int x, int y);

    void dealCard(std::pair<QPixmap*, uint8_t> &card_to_value,
                  BlackjackPlayer &blackjack_player,
                  int card_position_y);

    void animateCard(QPixmap *card, int x, int y);

    void updateScoreMessage(BlackjackPlayer &blackjack_player);
    void updateBlackjackPlayer(BlackjackPlayer &blackjack_player);

    void calculateResults();
    void compareScores();

    void draw();
    void playerWin();
    void playerLose();

    void __newRound();

private slots:
    void deal();
    void bet(int sum, bool add);
    void stand();
    void hit();
    void newRound();
};

#endif // GAMEMANAGER_H