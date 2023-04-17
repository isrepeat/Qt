#include "audio/gameaudio.h"

GameAudio::GameAudio()
    : bgMusicPlayer(std::make_unique<QMediaPlayer>()),
    audioOutput(std::make_unique<QAudioOutput>()),
    dealSound(loadSoundEffect("deal.wav")),
    betSound(loadSoundEffect("bet.wav")),
    clickSound(loadSoundEffect("click.wav")),
    winSound(loadSoundEffect("win.wav")),
    lossSound(loadSoundEffect("loss.wav")),
    pushSound(loadSoundEffect("push.wav"))
{
    bgMusicPlayer->setAudioOutput(audioOutput.get());
    bgMusicPlayer->setSource(QUrl("qrc:/sound/soundtrack.ogg"));
    bgMusicPlayer->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0.035f);
}

void GameAudio::enableSoundEffects(bool enable)
{
    soundEffectsEnabled = enable;
}

void GameAudio::playBgMusic()
{
    bgMusicPlayer->play();
}

bool GameAudio::isBgMusicPlaying()
{
    //return bgMusicPlayer->isPlaying();
    return false;
}

bool GameAudio::areSoundEffectsEnabled()
{
    return soundEffectsEnabled;
}

void GameAudio::playBetSound()
{
    playSound(*betSound);
}

void GameAudio::playClickSound()
{
    playSound(*clickSound);
}

void GameAudio::playDealSound()
{
    playSound(*dealSound);
}

void GameAudio::playWinSound()
{
    playSound(*winSound);
}

void GameAudio::playLossSound()
{
    playSound(*lossSound);
}

void GameAudio::playPushSound()
{
    playSound(*pushSound);
}

std::unique_ptr<QSoundEffect> GameAudio::loadSoundEffect(const QString &file)
{
    auto sound = std::make_unique<QSoundEffect>();
    sound->setSource(QUrl("qrc:/sound/" + file));
    sound->setVolume(0.3f);
    return sound;
}

void GameAudio::playSound(QSoundEffect &sound)
{
    if (soundEffectsEnabled)
        sound.play();
}

QAudioOutput& GameAudio::getAudioOutput()
{
    return *audioOutput;
}

QMediaPlayer& GameAudio::getBgMusicPlayer()
{
    return *bgMusicPlayer;
}

