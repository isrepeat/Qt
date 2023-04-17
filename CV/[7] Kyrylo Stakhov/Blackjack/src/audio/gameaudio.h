#ifndef GAMEAUDIO_H
#define GAMEAUDIO_H

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QSoundEffect>

class GameAudio
{
private:
    std::unique_ptr<QMediaPlayer> bgMusicPlayer;
    std::unique_ptr<QAudioOutput> audioOutput;

    std::unique_ptr<QSoundEffect>
        dealSound,
        betSound,
        clickSound,
        winSound,
        lossSound,
        pushSound;

    static std::unique_ptr<QSoundEffect> loadSoundEffect(const QString&);

    bool soundEffectsEnabled = true;

    void playSound(QSoundEffect&);

public:
    GameAudio();

    void enableSoundEffects(bool enable);
    void playBgMusic();
    bool isBgMusicPlaying();
    bool areSoundEffectsEnabled();

    void playBetSound();
    void playClickSound();
    void playDealSound();

    void playWinSound();
    void playLossSound();
    void playPushSound();

    QAudioOutput& getAudioOutput();
    QMediaPlayer& getBgMusicPlayer();
};

#endif // GAMEAUDIO_H
