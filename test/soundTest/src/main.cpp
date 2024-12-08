#include "Sound.hpp"
#include <time.h>

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "oct_binary.lib")
#pragma comment(lib, "oct_wav.lib")
#pragma comment(lib, "oct_game_d.lib")

void wait(time_t t) {
    time_t last_t = time(NULL);
    while(true) {
        time_t now_t = time(NULL);

        if(now_t - last_t >= 5) {
            return;
        }
    }
}

int main() {
    sound::SOUNDINTERFACE si;
    sound::SOUNDDATA sd;

    sound::CreateInterface(&si);
    sound::Load("anoband.wav", &sd);

    sound::SetVolume(&sd, 0.01);
    sound::Play(&sd);
    wait(5);
    sound::Stop(&sd);

    wait(1);

    sound::SetVolume(&sd, 0.02);
    sound::Play(&sd);
    wait(5);
    sound::Pause(&sd);

    wait(1);

    sound::SetVolume(&sd, 0.03);
    sound::Play(&sd);
    wait(5);
    sound::Stop(&sd);

    sound::DestroySoundData(&sd);
    sound::DestroyInterface(&si);

    return 0;
}
