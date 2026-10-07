#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

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

        LoopRecorder() = default;
        ~LoopRecorder();

        void init();

        const std::vector<AudioDevice>& getAudioDevices() const;
        std::vector<SavedFile> getSavedFiles() const;

        bool isRecording() const { return m_isRecording; }

        void startRecording(const AudioDevice* device);
        void stopRecording();

        void deleteFile(const SavedFile& file);

    private:

        void shutdown();

        void recordDevice(int deviceId);
        void saveRecordingToFile(const std::string& filename);

        void refreshAudioDevices();
        void refreshSavedFiles();

        bool m_initialized = false;
        std::atomic<bool> m_isRecording = false;

        std::thread m_recordingThread;

        std::vector<AudioDevice> m_audioDevices;

        mutable std::mutex m_savedFilesMutex;
        std::vector<SavedFile> m_savedFiles;

        std::vector<float> m_recordedSamples;

    };

}

