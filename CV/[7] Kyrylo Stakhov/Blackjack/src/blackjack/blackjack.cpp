#include "blackjack/blackjack.h"
#include "resources.h"

#include <QMessageBox>

BlackJack::BlackJack()
    : dealer(new Hand), player(new Hand) {}

bool BlackJack::startGame(int betAmount, Action afterStart)
{
    if (betAmount > balance || betAmount <= 0)
        return false;

    balance -= betAmount;
    bet = betAmount;
    startGame().schedule(afterStart, Hand::timeToDraw(2) * 1.9);
    return true;
}

void BlackJack::hit(Action afterHit)
{
    player->draw(deck, [this, afterHit] {
        auto score = player->evaluate();
        qDebug() << "Drew a card worth" << player->lastCard().getScore() << "; Total:" << score;
        if (score > BJ)
            state = GameState::Bust;
        afterHit();
    });
}

void BlackJack::stand(Action onGameEnd)
{
    dealer->lastCard().showFace();
    doLater([this, onGameEnd] {
        dealerAction(onGameEnd);
    }, DEALER_ACTION_DELAY);
}

void BlackJack::dealerAction(Action onGameEnd)
{
    if (dealer->evaluate() < dealerThreshold) {
        dealer->draw(deck, [this, onGameEnd]{
            dealerAction(onGameEnd);
        });
    }
    else {
        checkGameEndConditions();
        onGameEnd();
    }
}

int BlackJack::getBalance()
{
    return balance;
}

void BlackJack::setBalance(int balance)
{
    this->balance = balance;
}

int BlackJack::getBet()
{
    return bet;
}

BlackJack::GameState BlackJack::getGameState()
{
    return state;
}

bool BlackJack::isGameInProgress()
{
    return state == GameState::InProgress;
}

void BlackJack::resetBalance()
{
    balance = DEFAULT_BALANCE;
    bet = 0;
    state = GameState::None;
}

int BlackJack::winAmount()
{
    switch(state) {
    case GameState::BlackJack:
        return bet * 2.5;

    case GameState::Win:
        return bet * 2;

    case GameState::Push:
        return bet;

    default:
        return 0;
    }
}

Hand &BlackJack::getDealerHand()
{
    return *dealer;
}

Hand &BlackJack::getPlayerHand()
{
    return *player;
}

TaskChain BlackJack::startGame()
{
    state = GameState::InProgress;
    deck.reset();
    dealer->clear();
    player->clear();
    return TaskChain()
        .schedule([this] {dealer->draw(deck);})
        .schedule([this] {player->draw(deck);}, START_ACTION_DELAY)
        .schedule([this] {dealer->drawHidden(deck);}, START_ACTION_DELAY)
        .schedule([this] {player->draw(deck);}, START_ACTION_DELAY)
        .schedule([this] {
            if (player->evaluate() == BJ) {
                state = GameState::BlackJack;
                balance += winAmount();
            }
        });
}

void BlackJack::checkGameEndConditions()
{
    auto playerScore = player->evaluate();
    auto dealerScore = dealer->evaluate();

    qDebug() << "Ending the game. Dealer has:" << dealerScore << "; player has:" << playerScore;

    if (playerScore > BJ)
        state = GameState::Bust;

    else if (dealerScore > BJ || dealerScore < playerScore)
        state = GameState::Win;

    else if (dealerScore > playerScore)
        state = GameState::Loss;

    else if (dealerScore == playerScore)
        state = GameState::Push;

    balance += winAmount();
}

