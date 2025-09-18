#pragma once

#include "application/application.h"

#include <vector>
#include <string>
#include <assert.h>

class PaDeviceInfo;
class PaHostApiInfo;

namespace aiko::recorder
{

    class LoopRecorder
    {
    public:

        struct AudioDevice
        {
            using DeviceId = int;
            const DeviceId id;
            const PaDeviceInfo* info;
            const PaHostApiInfo* host;
        };

        struct SavedFile
        {
            std::string filename;
        };

        void init();

        std::vector<AudioDevice>& getAudioDevices();
        std::vector<SavedFile>& getSavedFiles();

        bool isRecording() const { return _isRecording; }

        void startRecording(const AudioDevice* device);
        void stopRecording();

        void deleteFile(SavedFile);

    protected:

    private:

        void recordDevice(int deviceId);
        void saveRecordingToFile(std::string filename);

        void refreshAudioDevices();
        void refreshSavedFiles();

        std::atomic<bool> _isRecording = false;
        std::vector<AudioDevice> m_audioDevices;
        std::vector<SavedFile> m_savedFiles;
        std::vector<float> m_recordedSamples;

    };

}

