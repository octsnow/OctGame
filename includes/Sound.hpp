#ifndef SOUND_HPP
#define SOUND_HPP

#include <al.h>
#include <alc.h>
#include <string>

namespace sound {
    typedef struct sound_interface_tag {
        ALCdevice *device;
        ALCcontext *context;
    } SOUNDINTERFACE;
    typedef struct sound_data_tag {
        ALuint buffer;
        ALuint source;
    } SOUNDDATA;
    void CreateInterface(SOUNDINTERFACE *pinterface);
    void Load(std::string filename, SOUNDDATA *pdata);
    void SetVolume(SOUNDDATA *pdata, float volume);
    void SetIsLooping(SOUNDDATA *pdata, bool isLooping);
    void Play(SOUNDDATA *pdata);
    void Pause(SOUNDDATA *pdata);
    void Stop(SOUNDDATA *pdata);
    void DestroyInterface(SOUNDINTERFACE *pinterface);
    void DestroySoundData(SOUNDDATA *pdata);
}

#endif
