#ifndef BLACKJACK_H
#define BLACKJACK_H

#include "types.h"
#include "blackjack/deck.h"
#include "blackjack/hand.h"

class BlackJack
{
public:
    enum class GameState
    {
        None,       // Only used as initial `state` value before the first game
        InProgress,
        Bust,       // When player's total is >21
        Loss,       // When dealer has a better hand
        Win,        // When player has a better hand
        BlackJack,  // When player's two initial cards are worth 21
        Push        // When player and dealer have equivalent hands
    };

    static constexpr int DEFAULT_BALANCE = 100;

private:
    GameState state = GameState::None;
    Deck deck;

    /* Dealer's & player's hands.
     * Safe to allocate via `new` because they will be owned by `QGraphicsScene`,
     * not by this class.
     */
    Hand *dealer;
    Hand *player;

    int balance = DEFAULT_BALANCE;
    int bet = 0;

    /* Score after which dealer stops drawing cards from the deck */
    size_t dealerThreshold = 17;

    TaskChain startGame();
    void dealerAction(Action onGameEnd);
    void checkGameEndConditions();

public:
    static constexpr int DEALER_ACTION_DELAY = 500;
    static constexpr int START_ACTION_DELAY = 250;

    BlackJack();

    bool startGame(int betAmount, Action afterStart);

    void hit(Action afterHit);
    void stand(Action onGameEnd);

    int getBalance();
    void setBalance(int balance);
    int getBet();

    GameState getGameState();
    bool isGameInProgress();
    void resetBalance();

    int winAmount();

    Hand& getDealerHand();
    Hand& getPlayerHand();

    static constexpr size_t BJ = 21;
};

#endif // BLACKJACK_H
