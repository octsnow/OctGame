#include "Sound.hpp"
#include <string>
#include <vector>

typedef struct audio_node_tag {
    std::string name;
    sound::SOUNDDATA data;
} AUDIONODE;

class Audio {
public:
    Audio();
    ~Audio();
    void Load(std::string filepath, std::string name);
    void Play(std::string name);
    void Pause(std::string name);
    void Stop(std::string name);
    void SetVolume(std::string name, float volume);
    void SetIsLooping(std::string name, bool isLooping);

private:
    sound::SOUNDINTERFACE m_sound_interface;
    std::vector<AUDIONODE> m_audios;
};
