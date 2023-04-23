#include "GameSceneManager.h"

GameSceneManager::~GameSceneManager() {
    delete _button_deal.data();
    delete _button_hit.data();
    delete _button_stand.data();
    delete _deal_text;
    delete _balance_text;
    delete _playerscore_text;
    delete _dealerscore_text;
    delete _gameresult_text;
    delete _timer;
}

void GameSceneManager::clearDealScene() {
    _scene->removeItem(_deal_text);
    _scene->removeItem(_balance_text);
    _scene->removeItem(_playerscore_text);
    _scene->removeItem(_dealerscore_text);
    delete _deal_text;
    delete _balance_text;
    delete _playerscore_text;
    delete _dealerscore_text;
    delete _gameresult_text;
    delete _timer;
    _deal_text = nullptr;
    _balance_text = nullptr;
    _playerscore_text = nullptr;
    _dealerscore_text = nullptr;
    _gameresult_text = nullptr;
    _timer = nullptr;
}

void GameSceneManager::deal() { 
    if (_bet_sum == 0) return;

    _scene->removeItem(_button_deal.get());
    for (auto &chip : _chips) {
        if (_scene->items().contains(chip)) {
            _scene->removeItem(chip);
        }
    }

    _deal_text->setPos(BETTED_TEXT_X, BETTED_TEXT_Y);
    _player->setBalance(_player->getBalance() - _bet_sum);
    addPlayerBalance();
    initHitStandScene();
}

void GameSceneManager::bet(int sum, bool add) {
    int player_balance = _player->getBalance();
    
    if (add && player_balance < _bet_sum + sum) return;
    if (!add && _bet_sum < sum) return;

    _bet_sum += (add ? sum : -sum);
    Sound::playChip();

    std::stringstream deal_text;
    deal_text << "Your bet: $" << _bet_sum;
    setMessage(&_deal_text, deal_text.str().c_str(), DEAL_TEXT_X, DEAL_TEXT_Y);
}

GameSceneManager::GameSceneManager(QGraphicsScene *scene, 
                                   int resolution_x, int resolution_y) {
    _scene            = scene;
    _bet_sum          = 0;
    _button_deal      = nullptr;
    _deal_text        = nullptr;
    _balance_text     = nullptr;
    _playerscore_text = nullptr;
    _dealerscore_text = nullptr;
    _gameresult_text  = nullptr;
    _timer            = nullptr;
    _resolution_x     = resolution_x;
    _resolution_y     = resolution_y;
    _dealer           = std::make_unique<Dealer>();
    _player           = std::make_unique<Player>();
    _font             = std::make_unique<QFont>();

    _pixmap_items.reserve(15);
    _chips.reserve(6);
    _font->setPointSize(FONT_SIZE);
}

void GameSceneManager::start() {
    initDealSceneButtons();
    initDealSceneChips();
    addDealSceneItems();
    addPlayerBalance(); 
}

void GameSceneManager::addDealSceneItems() {
    _bet_sum = 0;
    _scene->addItem(_button_deal.get());
    for (auto &chip : _chips) {
        _scene->addItem(chip);
    }
}

void GameSceneManager::initDealSceneButtons() {
    initButton(_button_deal, "Deal", 300);
    connect(_button_deal.get(), SIGNAL(clicked()), this, SLOT(deal()));
}

void GameSceneManager::initDealSceneChips() {
    _chips.clear();
    int x = 0;
    for (int chip_value : {1, 5, 25, 50, 100, 500}) {
        auto chip = new Chip(chip_value);
        chip->setPos(FIRST_CHIP_X + x * CHIP_HOVER_DIAMETER, CHIPS_HEIGHT);
        _chips.push_back(chip);

        connect(_chips.back(), &Chip::clicked, this, [=](bool add){
            emit bet(chip_value, add);
        });

        ++x;
    }
}

void GameSceneManager::initButton(QPointer<Button> &button,
                                  const char *button_text,
                                  int button_y, int button_x) {
    button = new Button(button_text);
    if (button_x == -1) {
        button_x = _resolution_x / 2.0f - button->boundingRect().width() / 2.0f;
    }
    button->setPos(button_x, button_y);
}

void GameSceneManager::addPlayerBalance() {
    std::stringstream balance_text;
    balance_text << "Balance: $" << _player->getBalance();
    setMessage(&_balance_text, balance_text.str().c_str(),
        BALANCE_TEXT_X, BALANCE_TEXT_Y);
}

void GameSceneManager::setMessage(QGraphicsTextItem **textitem,
                                  const char *textmessage,
                                  int x, int y) {
    if (*textitem != nullptr) {
        _scene->removeItem(*textitem);
        delete *textitem;
        *textitem = nullptr;
    }
    *textitem = _scene->addText(textmessage);
    (*textitem)->setFont(*_font);
    (*textitem)->setDefaultTextColor(Qt::white);
    (*textitem)->setPos(x, y);
}

void GameSceneManager::initHitStandScene() {
    _player->reset();
    _dealer->reset();

    initHitStandSceneButtons();

    int cards = 2;
    while (cards--) {
        updateBlackjackPlayer(*_player);
        updateBlackjackPlayer(*_dealer);
    }
    addHitStandSceneItems();
    calculateResults();
}

void GameSceneManager::updateBlackjackPlayer(BlackjackPlayer &blackjack_player) {
    auto card_to_value = _dealer->getCard();
    dealCard(card_to_value, blackjack_player,
        &blackjack_player == _player.get() ? PLAYERCARD_Y : DEALERCARD_Y);
    updateScoreMessage(blackjack_player);
}

void GameSceneManager::updateScoreMessage(BlackjackPlayer &blackjack_player) {
    const char *msg;
    int score_y;
    int score;
    QGraphicsTextItem **text;

    bool is_player = &blackjack_player == _player.get();

    if (is_player) {
        msg = "Your score: ";
        score_y = PLAYER_SCORE_Y;
        text = &_playerscore_text;
        score = blackjack_player.getHandScore();
    }
    else {
        msg = "Dealer score: ";
        score_y = DEALER_SCORE_Y;
        text = &_dealerscore_text;
        score = blackjack_player.standing()
              ? blackjack_player.getHandScore()
              : blackjack_player.getHandScore()   // hide first card score
                   - blackjack_player.getHandCards().front().second;
    }

    std::stringstream score_text;
    score_text << msg << score;
    setMessage(text, score_text.str().c_str(), SCORE_X, score_y);
}

void GameSceneManager::dealCard(std::pair<QPixmap*, uint8_t> &card_to_value,
                                BlackjackPlayer &blackjack_player,
                                int card_position_y) {
    auto card = card_to_value.first;
    auto cardvalue = card_to_value.second;
    auto cards_count = blackjack_player.getHandCardsCount();

    if (&blackjack_player == _dealer.get() && cards_count == 0) {
        card = _dealer->getUnknownCard().first;
    }
    _pixmap_items.push_back(_scene->addPixmap(*card));


    animateCard(card, CARD_X + (cards_count * card->width() / 2), card_position_y);
    blackjack_player.dealCard(card_to_value);
}

void GameSceneManager::animateCard(QPixmap *card, int x, int y) {
    QPointF start_position(-200, -200);
    QPointF final_position(x, y);

    auto animation = new QGraphicsItemAnimation();
    animation->setItem(_pixmap_items.back());

    auto timeline = new QTimeLine(500);
    animation->setTimeLine(timeline);

    animation->setPosAt(0, start_position);
    animation->setPosAt(1, final_position);

    connect(timeline, &QTimeLine::finished, animation, &QObject::deleteLater);
    connect(timeline, &QTimeLine::finished, timeline, &QObject::deleteLater);

    timeline->start();
    Sound::playCardDeal();
}

void GameSceneManager::initHitStandSceneButtons() {
    initButton(_button_hit, "Hit", BUTTON_HIT_Y, BUTTON_STAND_HIT_X);
    initButton(_button_stand, "Stand", BUTTON_STAND_Y, BUTTON_STAND_HIT_X);
    connect(_button_hit.get(), SIGNAL(clicked()), this, SLOT(hit()));
    connect(_button_stand.get(), SIGNAL(clicked()), this, SLOT(stand()));
}

void GameSceneManager::addHitStandSceneItems() {
    _scene->addItem(_button_hit.get());
    _scene->addItem(_button_stand.get());
}

void GameSceneManager::stand() {
    _player->stand();

    while (_dealer->getHandScore() < 16)
        updateBlackjackPlayer(*_dealer);

    _dealer->stand();

    auto redraw_cards = [&]() {
        int card_index = 0;
        for (auto &card : _dealer->getHandCards()) {
            auto item = _scene->addPixmap(*card.first);
            item->setPos(
                CARD_X + (card_index * card.first->width() / 2),
                DEALERCARD_Y);

            _pixmap_items.push_back(item);
            ++card_index;
        }
    };

    redraw_cards();
    updateScoreMessage(*_dealer);

    calculateResults();
}

void GameSceneManager::hit() {
    updateBlackjackPlayer(*_player);
    calculateResults();
}

void GameSceneManager::calculateResults() {
    int player_score = _player->getHandScore();
    int dealer_score = _dealer->getHandScore();

    if (dealer_score >= 16 || player_score > 21)
        _dealer->stand();

    if (player_score >= 21 || dealer_score > 21)
        _player->stand();

    if (!(_player->standing() && _dealer->standing()))
        return; // continue play

    compareScores();
}

void GameSceneManager::compareScores() {
    int player_score = _player->getHandScore();
    int dealer_score = _dealer->getHandScore();

    if (player_score == dealer_score)
        draw();
    else if (((player_score > dealer_score) || (dealer_score > 21))
                        && (player_score <= 21))
        playerWin();
    else
        playerLose();

    _timer = new QTimer;
    connect(_timer, &QTimer::timeout, this, &GameSceneManager::newRound);
    _timer->start(2000); // wait 2 sec and then start new round

    _scene->removeItem(_button_hit);
    _scene->removeItem(_button_stand);
}

void GameSceneManager::draw() {
    setMessage(&_gameresult_text, "DRAW", GAMERESULT_X, GAMERESULT_Y);
    _player->setBalance(_player->getBalance() + _bet_sum);
    _gameresult_text->setDefaultTextColor(Qt::yellow);
}

void GameSceneManager::playerWin() {
    _player->setBalance(_player->getBalance() + _bet_sum * 2);
    setMessage(&_gameresult_text, "WIN", GAMERESULT_X, GAMERESULT_Y);
    _gameresult_text->setDefaultTextColor(Qt::green);
}

void GameSceneManager::playerLose() {
    setMessage(&_gameresult_text, "LOSE", GAMERESULT_X, GAMERESULT_Y);
    _gameresult_text->setDefaultTextColor(Qt::red);
}

void GameSceneManager::newRound() {
    __newRound();
}

void GameSceneManager::__newRound() {
    for (auto &pixmap : _pixmap_items) {
        if (_scene->items().contains(pixmap)) {
            _scene->removeItem(pixmap);
        }
    }
    _pixmap_items.clear();
    clearDealScene();

    addPlayerBalance();
    addDealSceneItems();
}