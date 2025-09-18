#include "loop_recorder.h"

#include <aiko_includes.h>

#include <iostream>
#include <filesystem>
#include <portaudio.h>
#include <thread>
#include <fstream>

namespace aiko::recorder
{
    void LoopRecorder::init()
    {
        static_assert(std::is_same_v<PaDeviceIndex, AudioDevice::DeviceId>, "PaDeviceIndex and AudioDevice::DeviceId must be the same type");
        Pa_Initialize();
        refreshAudioDevices();
        refreshSavedFiles();
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

        std::cout << "Found " << numDevices << " audio devices:\n\n";

        for (int i = 0; i < numDevices; i++)
        {

            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

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

    }

    void LoopRecorder::refreshSavedFiles()
    {
        try
        {
            const std::string extension = ".wav";

            m_savedFiles.clear();
            for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path().c_str()))
            {
                if (entry.is_regular_file())
                {
                    std::string filename = entry.path().filename().string();
                    if (extension.empty() || entry.path().extension() == extension)
                    {
                        m_savedFiles.push_back({ filename});
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }

    void LoopRecorder::deleteFile(LoopRecorder::SavedFile file)
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

    std::vector<LoopRecorder::AudioDevice>& LoopRecorder::getAudioDevices()
    {
        return m_audioDevices;
    }

    std::vector<LoopRecorder::SavedFile>& LoopRecorder::getSavedFiles()
    {
        return m_savedFiles;
    }

    void LoopRecorder::startRecording(const AudioDevice* device)
    {
        assert(_isRecording == false);
        std::thread(&LoopRecorder::recordDevice, this, device->id).detach();
    }

    void LoopRecorder::stopRecording()
    {
        assert(_isRecording == true);
        _isRecording = false;
    }

    void LoopRecorder::recordDevice(int deviceId)
    {

        auto recordCallback = [](const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) -> int
        {
            std::vector<float>* recordedSamples = (std::vector<float>*)userData;
            const float* input = (const float*)inputBuffer;
            for (unsigned int i = 0; i < framesPerBuffer; i++)
            {
                recordedSamples->push_back(input[i]);
            }
            return paContinue;
        };


        m_recordedSamples.clear();

        PaStream* stream;

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
            return;
        }

        _isRecording = true;
        Pa_StartStream(stream);
        std::cout << "Recording started..." << std::endl;

        while (_isRecording)
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

    void LoopRecorder::saveRecordingToFile(std::string filename)
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

