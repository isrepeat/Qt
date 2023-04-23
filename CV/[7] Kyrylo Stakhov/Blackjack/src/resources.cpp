#include "resources.h"

#include "graphics/spritesheet.h"
#include "util/dialogs.h"
#include "blackjack/card.h"

#include <QFile>

std::shared_ptr<SpriteSheet>
    Resources::cardSheet,
    Resources::defaultCardSheet;

std::unique_ptr<QPixmap> Resources::gameBackground;

QString
    Resources::balanceString("Balance: $%1"),
    Resources::betString("Bet: $%1"),
    Resources::handString("Hand score: %1"),
    Resources::bustMsg("You busted and lost your bet of <span style='color:red'>$%1</span>."),
    Resources::winMsg("Congratulations! You won <span style='color:green'>$%1</span>!"),
    Resources::lossMsg("You lost your bet of <span style='color:red'>$%1</span>."),
    Resources::pushMsg("Dealer has an equivalent hand. Your bet is refunded.");

std::unique_ptr<GameAudio> Resources::audio;

void Resources::load()
{
    if (cardSheet)
        return;

    loadCardSheet(":/textures/cards.png");
    gameBackground = std::make_unique<QPixmap>(":/textures/bg.png");
    defaultCardSheet = cardSheet;
    audio = std::make_unique<GameAudio>();
}

QString Resources::readFile(const QString &path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    return {file.readAll()};
}

void Resources::loadCardSheet(const QString &path)
{
    auto newSheet = std::make_shared<SpriteSheet>(
        path,
        Card::RANKS,
        Card::SUITS + 1 /* All suits + backside */
    );
    if (newSheet->getRegionWidth() > 0 && newSheet->getRegionHeight() > 0)
        cardSheet = newSheet;
    else
        Dialog::error("Error", "Invalid card skin file!");
}

void Resources::resetCardSheet()
{
    cardSheet = defaultCardSheet;
}
