#include "../headers/audio.h"


AudioPlayer::AudioPlayer() {
    device = audiere::OpenDevice();
}

AudioPlayer::~AudioPlayer() {
    stop();
}

bool AudioPlayer::load(const std::filesystem::path& path) {
    
    if (!device) {
        return false;
    }
    
    stream = audiere::OpenSound(device.get(), path.string().c_str(), true);
    
    if (!stream) {
        return false;
    }
    return true;
}

void AudioPlayer::play() {
    if (stream) {
        
        stream->play();
    }
}

void AudioPlayer::pause() {
    if (stream) {
        stream->stop();
    }
}

void AudioPlayer::resume() {
    if (stream) {
        stream->play();
    }
}

void AudioPlayer::stop() {
    if (stream) {
        stream->stop();
    }
}

float AudioPlayer::getVolume() const {
    if (stream) {
        return stream->getVolume();
    }
    return 0.0f;
}

float AudioPlayer::getPosition() {
    if( stream) {
        return stream->getPosition();
    }
}
void AudioPlayer::setVolume(float volume) {
    if (stream) {
        stream->setVolume(volume);
    }
}

void AudioPlayer::setPosition(int samples) {
    if ( stream) {
        stream ->setPosition(samples);
    }
}

bool AudioPlayer::isPlaying() const {
    if (stream) {
        return stream->isPlaying();
    }
    return false;
}