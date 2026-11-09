#include "wolf_audio_player.h"

#include "aiko_macros.h"

namespace wc
{

    void WolfAudioPlayer::init(std::vector<WolfDigitizedSound> sounds)
    {
        m_sounds = std::move(sounds);
        const ma_result result = ma_engine_init(nullptr, &m_engine);
        AIKO_ASSERT(result == MA_SUCCESS, "Could not initialize audio engine");
        m_engineInitialized = true;
    }

    void WolfAudioPlayer::play(size_t soundIdx)
    {
        if (m_engineInitialized == false || soundIdx >= m_sounds.size() || m_sounds[soundIdx].samples.empty())
        {
            return;
        }

        stop();

        const WolfDigitizedSound& sound = m_sounds[soundIdx];

        ma_audio_buffer_config config = ma_audio_buffer_config_init(ma_format_u8, 1, sound.samples.size(), sound.samples.data(), nullptr);

        config.sampleRate = sound.sampleRate;

        const ma_result bufferResult = ma_audio_buffer_init(&config, &m_buffer);
        AIKO_ASSERT(bufferResult == MA_SUCCESS, "Could not initialize audio buffer");
        m_bufferInitialized = true;

        const ma_result soundResult = ma_sound_init_from_data_source(&m_engine, &m_buffer, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &m_sound);

        AIKO_ASSERT(soundResult == MA_SUCCESS, "Could not initialize sound");
        m_soundInitialized = true;

        const ma_result playResult = ma_sound_start(&m_sound);
        AIKO_ASSERT(playResult == MA_SUCCESS, "Could not start sound playback");
    }

    void WolfAudioPlayer::stop()
    {
        if (m_soundInitialized)
        {
            ma_sound_uninit(&m_sound);
            m_soundInitialized = false;
        }
        if (m_bufferInitialized)
        {
            ma_audio_buffer_uninit(&m_buffer);
            m_bufferInitialized = false;
        }
    }

    void WolfAudioPlayer::shutdown()
    {
        stop();
        if (m_engineInitialized)
        {
            ma_engine_uninit(&m_engine);
            m_engineInitialized = false;
        }
    }

}