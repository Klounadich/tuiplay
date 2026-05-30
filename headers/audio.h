#pragma once
#include <filesystem>
#include <thread>
#include <atomic>
#include <mutex>
#include <audiere.h>


#include <SDL2/SDL.h>


class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();
    
    bool load(const std::filesystem::path& path);
    void play();
    void pause();
    void resume();
    void stop();
    
    
    float getVolume() const;
    void setPosition( int samples) ;
    float getPosition();
    void setVolume(float volume);
    bool isPlaying() const;
   

private:
    void cleanup();
    
    audiere::AudioDevicePtr device;
    audiere::OutputStreamPtr stream;
    
    std::atomic<bool> is_playing{false};
    std::atomic<bool> should_stop{false};
    std::thread playback_thread;
    mutable std::mutex mutex;
    
   
};
