#include "Audio.hpp"
#include <stdio.h>

namespace {
    int search(std::vector<AUDIONODE> audios, std::string name) {
        for(int i = 0; i < audios.size(); i++) {
            if(audios[i].name == name) {
                return i;
            }
        }

        return -1;
    }
}

Audio::Audio() {
    sound::CreateInterface(&this->m_sound_interface);
}

Audio::~Audio() {
    sound::DestroyInterface(&this->m_sound_interface);
    for(AUDIONODE an : this->m_audios) {
        sound::DestroySoundData(&an.data);
    }
}

void Audio::Load(std::string filepath, std::string name) {
    AUDIONODE new_an;

    for(AUDIONODE an : this->m_audios) {
        if(an.name == name) {
            printf("invalid name");
            return;
        }
    }
    
    new_an.name = name;
    sound::Load(filepath, &new_an.data);

    this->m_audios.push_back(new_an);
}

void Audio::Play(std::string name) {
    int i = search(this->m_audios, name);

    if(i == -1) {
        return;
    }

    sound::Play(&this->m_audios[i].data);
}

void Audio::Pause(std::string name) {
    int i = search(this->m_audios, name);

    if(i == -1) {
        return;
    }

    sound::Pause(&this->m_audios[i].data);
}

void Audio::Stop(std::string name) {
    int i = search(this->m_audios, name);

    if(i == -1) {
        return;
    }

    sound::Stop(&this->m_audios[i].data);
}

void Audio::SetVolume(std::string name, float volume) {
    int i = search(this->m_audios, name);

    if(i == -1) {
        return;
    }

    sound::SetVolume(&this->m_audios[i].data, volume);
}

void Audio::SetIsLooping(std::string name, bool isLooping) {
    int i = search(this->m_audios, name);

    if(i == -1) {
        return;
    }

    sound::SetIsLooping(&this->m_audios[i].data, isLooping);
}
