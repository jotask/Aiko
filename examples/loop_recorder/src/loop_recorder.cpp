#include "loop_recorder.h"

#include <portaudio.h>

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <string_view>
#include <utility>

namespace aiko::recorder
{

    LoopRecorder::~LoopRecorder()
    {
        shutdown();
    }

    void LoopRecorder::init()
    {
        static_assert(std::is_same_v<PaDeviceIndex, AudioDevice::DeviceId>, "PaDeviceIndex and AudioDevice::DeviceId must be the same type");

        if (m_initialized)
        {
            return;
        }

        const PaError error = Pa_Initialize();
        if (error != paNoError)
        {
            std::cerr << "Failed to initialize PortAudio: " << Pa_GetErrorText(error) << '\n';
            return;
        }

        m_initialized = true;

        refreshAudioDevices();
        refreshSavedFiles();
    }

    void LoopRecorder::shutdown()
    {
        if (m_recordingThread.joinable())
        {
            m_isRecording = false;
            m_recordingThread.join();
        }

        if (m_initialized)
        {
            Pa_Terminate();
            m_initialized = false;
        }
    }

    void LoopRecorder::refreshAudioDevices()
    {

        constexpr bool enable_COUT = false;

        m_audioDevices.clear();

        int numDevices = Pa_GetDeviceCount();
        if (numDevices < 0)
        {
            std::cerr << "ERROR: No audio devices found! " << Pa_GetErrorText(numDevices) << "\n";
            return;
        }

        for (int i = 0; i < numDevices; i++)
        {

            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

            if (deviceInfo == nullptr || deviceInfo->maxInputChannels <= 0)
            {
                continue;
            }

            const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

            if (hostApiInfo == nullptr)
            {
                continue;
            }

            m_audioDevices.push_back({ i, deviceInfo, hostApiInfo });

            if constexpr (enable_COUT)
            {
                std::cout << "Device #" << i << " - " << deviceInfo->name << " (" << hostApiInfo->name << ")\n";
                std::cout << "  Max Input Channels:  " << deviceInfo->maxInputChannels << "\n";
                std::cout << "  Max Output Channels: " << deviceInfo->maxOutputChannels << "\n";
                std::cout << "  Default Sample Rate: " << deviceInfo->defaultSampleRate << " Hz\n";
                std::cout << "  Latency (Input):  " << deviceInfo->defaultLowInputLatency * 1000 << " ms\n";
                std::cout << "  Latency (Output): " << deviceInfo->defaultLowOutputLatency * 1000 << " ms\n";
                std::cout << "--------------------------------------\n";
            }
        }

        std::cout << "Found " << m_audioDevices.size() << " input audio devices.\n";

    }

    void LoopRecorder::refreshSavedFiles()
    {
        try
        {
            std::vector<SavedFile> savedFiles;

            constexpr std::string_view extension = ".wav";

            for (const auto& entry : std::filesystem::directory_iterator( std::filesystem::current_path()))
            {
                if (entry.is_regular_file() && entry.path().extension() == extension)
                {
                    savedFiles.push_back(
                        { entry.path().filename().string() }
                    );
                }
            }

            std::scoped_lock lock(m_savedFilesMutex);
            m_savedFiles = std::move(savedFiles);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Filesystem error: " << e.what() << '\n';
        }
    }

    void LoopRecorder::deleteFile(const SavedFile& file)
    {
        try
        {
            std::filesystem::remove(file.filename);
            std::cout << "File deleted: " << file.filename << std::endl;
            refreshSavedFiles();
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Failed to delete file: " << e.what() << std::endl;
        }
    }

    const std::vector<LoopRecorder::AudioDevice>& LoopRecorder::getAudioDevices() const
    {
        return m_audioDevices;
    }

    std::vector<LoopRecorder::SavedFile> LoopRecorder::getSavedFiles() const
    {
        std::scoped_lock lock(m_savedFilesMutex);
        return m_savedFiles;
    }

    void LoopRecorder::startRecording(const AudioDevice* device)
    {
        if (device == nullptr || m_initialized == false || m_isRecording)
        {
            return;
        }

        if (m_recordingThread.joinable())
        {
            m_recordingThread.join();
        }

        m_isRecording = true;

        m_recordingThread = std::thread(&LoopRecorder::recordDevice, this, device->id);
    }

    void LoopRecorder::stopRecording()
    {
        m_isRecording = false;
    }

    void LoopRecorder::recordDevice(int deviceId)
    {

        auto recordCallback = [](const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) -> int
        {
            auto* recordedSamples = static_cast<std::vector<float>*>(userData);

            if (inputBuffer == nullptr)
            {
                recordedSamples->insert(recordedSamples->end(), framesPerBuffer, 0.0f);
                return paContinue;
            }
            const auto* input = static_cast<const float*>(inputBuffer);

            recordedSamples->insert(recordedSamples->end(), input, input + framesPerBuffer);

            return paContinue;
        };


        m_recordedSamples.clear();

        PaStream* stream = nullptr;

        PaStreamParameters inputParams;
        inputParams.device = deviceId;
        inputParams.channelCount = 1;
        inputParams.sampleFormat = paFloat32;
        inputParams.suggestedLatency = Pa_GetDeviceInfo(deviceId)->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = nullptr;

        PaError err = Pa_OpenStream(&stream, &inputParams, nullptr, 44100, 256, paClipOff, recordCallback, &m_recordedSamples);
        if (err != paNoError)
        {
            std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
            m_isRecording = false;
            return;
        }

        err = Pa_StartStream(stream);

        if (err != paNoError)
        {
            std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << '\n';

            Pa_CloseStream(stream);
            m_isRecording = false;
            return;
        }

        std::cout << "Recording started..." << std::endl;

        while (m_isRecording)
        {
            Pa_Sleep(100);
        }

        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        std::cout << "Recording stopped. Captured " << m_recordedSamples.size() << " samples." << std::endl;

        static auto generateTimestampFilename = []() -> std::string
        {
            std::time_t now = std::time(nullptr);
            std::tm* timeInfo = std::localtime(&now);

            std::ostringstream oss;
            oss << std::put_time(timeInfo, "%Y%m%d_%H%M%S") << ".wav";

            return oss.str();
        };

        saveRecordingToFile(generateTimestampFilename());

        refreshSavedFiles();

    }

    void LoopRecorder::saveRecordingToFile(const std::string& filename)
    {

        std::ofstream file(filename, std::ios::binary);

        int sampleRate = 44100;
        int numSamples = m_recordedSamples.size();
        int byteRate = sampleRate * sizeof(float);
        int dataSize = numSamples * sizeof(float);

        // WAV Header
        file.write("RIFF", 4);
        int chunkSize = 36 + dataSize;
        file.write(reinterpret_cast<const char*>(&chunkSize), 4);
        file.write("WAVEfmt ", 8);
        int subChunk1Size = 16;
        file.write(reinterpret_cast<const char*>(&subChunk1Size), 4);
        short audioFormat = 3;  // PCM floating point
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        short numChannels = 1;
        file.write(reinterpret_cast<const char*>(&numChannels), 2);
        file.write(reinterpret_cast<const char*>(&sampleRate), 4);
        file.write(reinterpret_cast<const char*>(&byteRate), 4);
        short blockAlign = sizeof(float);
        file.write(reinterpret_cast<const char*>(&blockAlign), 2);
        short bitsPerSample = 32;
        file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
        file.write("data", 4);
        file.write(reinterpret_cast<const char*>(&dataSize), 4);

        // Write audio data
        file.write(reinterpret_cast<const char*>(m_recordedSamples.data()), dataSize);
        file.close();

        std::cout << "Saved to " << filename << std::endl;
    }

}

