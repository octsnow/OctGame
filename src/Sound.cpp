#include "Sound.hpp"
#include "Wav.hpp"

namespace {
    uint8_t* ConvertTo16bit(wav::WAVEINFO* wi, uint8_t* data) {
        uint16_t sample_size = wi->info.pcm_waveformat.w_bits_per_sample / 8;
        uint32_t sample_num = wi->data_size / sample_size;
        uint8_t* new_data = (uint8_t*)malloc(sample_num * 2);

        for(int i = 0; i < sample_num; i++) {
            uint64_t buffer = 0;
            for(int j = 0; j < sample_size; j++) {
                buffer = (buffer << 8) | data[i * sample_size + (sample_size - j - 1)];
            }

            buffer ^= 1 << (sample_size * 8 - 1);
            buffer = (double)buffer / ((1 << (sample_size * 8)) - 1) * 65535 - 32768;
            new_data[i * 2    ] = (buffer     ) & 0xFF;
            new_data[i * 2 + 1] = (buffer >> 8) & 0xFF;
        }

        wi->data_size = sample_num * 2;

        return new_data;
    }
}

void sound::CreateInterface(sound::SOUNDINTERFACE *pinterface) {
    pinterface->device = alcOpenDevice(NULL);
    pinterface->context = alcCreateContext(pinterface->device, NULL);
    alcMakeContextCurrent(pinterface->context);
}

void sound::Load(std::string filename, sound::SOUNDDATA* pdata) {
    uint8_t* wav_data = NULL;
    wav::WAVEINFO wi;
    ALsizei sampling_rate = 0;
    ALenum bit_per_sample;

    if(wav::read(filename, &wav_data, &wi) == FAILED) {
        printf("reading wav is failed");
        if(wav_data != NULL) {
            free(wav_data);
        }

        return;
    }

    if(wi.format_type == wav::WaveFormatType::WAVEFORMAT) {
        sampling_rate = wi.info.waveformat.n_samples_per_sec;
        bit_per_sample = AL_FORMAT_MONO8;
    } else if(wi.format_type == wav::WaveFormatType::PCMWAVEFORMAT) {
        sampling_rate = wi.info.pcm_waveformat.wf.n_samples_per_sec;
        if(wi.info.pcm_waveformat.w_bits_per_sample == 8) {
            if(wi.info.pcm_waveformat.wf.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO8;
            } else {
                bit_per_sample = AL_FORMAT_MONO8;
            }
        } else if(wi.info.pcm_waveformat.w_bits_per_sample == 16) {
            if(wi.info.pcm_waveformat.wf.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO16;
            } else {
                bit_per_sample = AL_FORMAT_MONO16;
            }
        } else {
            uint8_t* new_data = ConvertTo16bit(&wi, wav_data);
            free(wav_data);
            wav_data = new_data;

            if(wi.info.pcm_waveformat.wf.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO16;
            } else {
                bit_per_sample = AL_FORMAT_MONO16;
            }
        }
    } else if(wi.format_type == wav::WaveFormatType::WAVEFORMATEX) {
        sampling_rate = wi.info.waveformat_ex.n_samples_per_sec;
        if(wi.info.waveformat_ex.w_bits_per_sample == 8) {
            if(wi.info.waveformat_ex.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO8;
            } else {
                bit_per_sample = AL_FORMAT_MONO8;
            }
        } else if(wi.info.waveformat_ex.w_bits_per_sample == 16) {
            if(wi.info.waveformat_ex.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO16;
            } else {
                bit_per_sample = AL_FORMAT_MONO16;
            }
        } else {
            uint8_t* new_data = ConvertTo16bit(&wi, wav_data);
            free(wav_data);
            wav_data = new_data;

            if(wi.info.pcm_waveformat.wf.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO16;
            } else {
                bit_per_sample = AL_FORMAT_MONO16;
            }
        }
    } else if(wi.format_type == wav::WaveFormatType::WAVEFORMATEXTENSIBLE) {
        sampling_rate = wi.info.waveformat_extensible.format.n_samples_per_sec;
        if(wi.info.waveformat_extensible.format.w_bits_per_sample == 8) {
            if(wi.info.waveformat_extensible.format.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO8;
            } else {
                bit_per_sample = AL_FORMAT_MONO8;
            }
        } else if(wi.info.waveformat_extensible.format.w_bits_per_sample == 16) {
            if(wi.info.waveformat_extensible.format.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO16;
            } else {
                bit_per_sample = AL_FORMAT_MONO16;
            }
        } else {
            uint8_t* new_data = ConvertTo16bit(&wi, wav_data);
            free(wav_data);
            wav_data = new_data;

            if(wi.info.pcm_waveformat.wf.n_channels == 2) {
                bit_per_sample = AL_FORMAT_STEREO16;
            } else {
                bit_per_sample = AL_FORMAT_MONO16;
            }
        }
    }

	alGenBuffers(1, &pdata->buffer);
	alGenSources(1, &pdata->source);

	alBufferData(pdata->buffer, bit_per_sample, wav_data, wi.data_size, sampling_rate);
	alSourcei(pdata->source, AL_BUFFER, pdata->buffer);

    if(wav_data != NULL) {
        free(wav_data);
    }
}

void sound::SetVolume(SOUNDDATA* pdata, float volume) {
    if(pdata == NULL) {
        return;
    }

    alSourcef(pdata->source, AL_GAIN, volume);
}

void sound::SetIsLooping(SOUNDDATA *pdata, bool isLooping) {
    if(pdata == NULL) {
        return;
    }

    alSourcei(pdata->source, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
}

void sound::Play(SOUNDDATA* pdata) {
    if(pdata == NULL) {
        return;
    }

	alSourcePlay(pdata->source);
}

void sound::Pause(sound::SOUNDDATA *pdata) {
    if(pdata == NULL) {
        return;
    }

    alSourcePause(pdata->source);
}

void sound::Stop(sound::SOUNDDATA *pdata) {
    if(pdata == NULL) {
        return;
    }

    alSourceStop(pdata->source);
}

void sound::DestroyInterface(sound::SOUNDINTERFACE *pinterface) {
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(pinterface->context);
	alcCloseDevice(pinterface->device);
}

void sound::DestroySoundData(sound::SOUNDDATA* pdata) {
    if(pdata == NULL) {
        return;
    }

	alDeleteBuffers(1, &pdata->buffer);
	alDeleteSources(1, &pdata->source);
}
