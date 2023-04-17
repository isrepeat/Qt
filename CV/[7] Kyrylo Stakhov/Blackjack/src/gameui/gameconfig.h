#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QSettings>

class BJWindow;

class GameConfig
{
private:
    static const QString
        BALANCE,
        BGM_ENABLED,
        SFX_ENABLED,
        SKIN_PATH;

    BJWindow &game;
    QSettings config{"bj.ini", QSettings::IniFormat};

public:
    GameConfig(BJWindow&);

    void load();
    void save();

    void customSkinLoaded(const QString&);
    void customSkinUnloaded();
};

#endif // GAMECONFIG_H
