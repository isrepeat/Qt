#include "gameconfig.h"
#include "gameui/bjwindow.h"
#include "resources.h"

const QString
    GameConfig::BALANCE("balance"),
    GameConfig::BGM_ENABLED("bgm_enabled"),
    GameConfig::SFX_ENABLED("sfx_enabled"),
    GameConfig::SKIN_PATH("skin");

GameConfig::GameConfig(BJWindow &game)
    :game(game) {}

void GameConfig::load()
{
    game.getBlackJack().setBalance(
        config.value(BALANCE, BlackJack::DEFAULT_BALANCE).toInt()
    );
    if (config.value(BGM_ENABLED, true).toBool())
        Resources::audio->playBgMusic();
    Resources::audio->enableSoundEffects(
        config.value(SFX_ENABLED, true).toBool()
    );
    auto skinPath = config.value(SKIN_PATH, "").toString();
    if (!skinPath.isEmpty()) {
        game.changeCardSkin(skinPath);
    }
}

void GameConfig::save()
{
    config.setValue(BALANCE, game.getBlackJack().getBalance());
    config.setValue(BGM_ENABLED, Resources::audio->isBgMusicPlaying());
    config.setValue(SFX_ENABLED, Resources::audio->areSoundEffectsEnabled());
}

void GameConfig::customSkinLoaded(const QString &path)
{
    config.setValue(SKIN_PATH, path);
}

void GameConfig::customSkinUnloaded()
{
    config.setValue(SKIN_PATH, "");
}

