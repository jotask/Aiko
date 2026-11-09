#pragma once

#include "assets/wolf_vswap_types.h"

#include <miniaudio.h>

namespace wc
{
    class WolfAudioPlayer final
    {
    public:
        void init(std::vector<WolfDigitizedSound>);
        void play(size_t);
        void stop();
        void shutdown();

        size_t getSoundsSize() const { return m_sounds.size(); }

    private:
        std::vector<WolfDigitizedSound> m_sounds;

        ma_engine m_engine{};
        ma_audio_buffer m_buffer{};
        ma_sound m_sound{};

        bool m_engineInitialized = false;
        bool m_bufferInitialized = false;
        bool m_soundInitialized = false;

    };
}
