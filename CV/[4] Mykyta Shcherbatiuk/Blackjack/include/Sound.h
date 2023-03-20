#ifndef SOUND_H
#define SOUND_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <cassert>
#include <memory>

class Sound {
public:
    Sound();
    static void playClick();
    static void playChip();
    static void playCardDeal();

private:
    std::unique_ptr<QMediaPlayer> _player;
    std::unique_ptr<QAudioOutput> _output;

    static Sound *_instance;
};

#endif // SOUND_H
