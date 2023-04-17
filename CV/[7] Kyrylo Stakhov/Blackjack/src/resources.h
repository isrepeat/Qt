#ifndef RESOURCES_H
#define RESOURCES_H

#include "audio/gameaudio.h"

#include <QString>
#include <memory>

class SpriteSheet;

class Resources
{
private:
    friend class BJWindow;
    static void load();

    static std::shared_ptr<SpriteSheet> defaultCardSheet;

public:
    static std::shared_ptr<SpriteSheet> cardSheet;

    static std::unique_ptr<QPixmap> gameBackground;

    static QString
        balanceString,
        betString,
        handString,

        bustMsg,
        winMsg,
        lossMsg,
        pushMsg;

    static std::unique_ptr<GameAudio> audio;

    static QString readFile(const QString&);

    static void loadCardSheet(const QString&);
    static void resetCardSheet();
};

#endif // RESOURCES_H
