#include "Sound.h"

Sound *Sound::_instance = nullptr;

Sound::Sound() {
    assert(_instance == nullptr);
    _instance = this;

    _player = std::make_unique<QMediaPlayer>();
    _output = std::make_unique<QAudioOutput>();

    _player->setAudioOutput(_output.get());
    _output->setVolume(50);
}

void Sound::playClick() {
    //_instance->_player->setSource(QUrl("qrc:/sound/click"));
    //_instance->_player->play();
}

void Sound::playChip() {
    //_instance->_player->setSource(QUrl("qrc:/sound/chip"));
    //_instance->_player->play();
}

void Sound::playCardDeal() {
    //_instance->_player->setSource(QUrl("qrc:/sound/card"));
    //_instance->_player->play();
}