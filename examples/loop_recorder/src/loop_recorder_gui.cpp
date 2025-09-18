#include "loop_recorder_gui.h"

#include <aiko_includes.h>

#include <imgui.h>
#include <portaudio.h>

namespace aiko::recorder
{
    void LoopRecorderGui::init()
    {
        Application::init();
        m_loopRecorder.init();
        m_loopVisualizer.init();
        m_loopPlayer.init();
    }

    void LoopRecorderGui::update()
    {
        Application::update();
    }

    void LoopRecorderGui::render()
    {
        Application::render();

        if (ImGui::Begin("Loop Recorder"))
        {
            static const LoopRecorder::AudioDevice* current_item = nullptr;
            ImGui::PushID(this);

            std::vector<LoopRecorder::AudioDevice>& audioDevices = m_loopRecorder.getAudioDevices();

            if (ImGui::BeginCombo("##combo", current_item ? current_item->info->name : "Select Device"))
            {
                for (int n = 0; n < audioDevices.size(); n++)
                {
                    ImGui::PushID(&audioDevices[n]);
                    bool is_selected = (current_item == &audioDevices[n]);

                    if (ImGui::Selectable(audioDevices[n].info->name, is_selected))
                    {
                        current_item = &audioDevices[n];
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();

            const bool IsRecording = m_loopRecorder.isRecording();

            if (ImGui::Button(IsRecording ? "Stop Recording" : "Start Recording") == true)
            {
                if (IsRecording)
                {
                    m_loopRecorder.stopRecording();
                }
                else
                {
                    if (current_item != nullptr)
                    {
                        m_loopRecorder.startRecording(current_item);
                    }
                }
            }

            {

                std::vector<LoopRecorder::SavedFile> recordedFiles = m_loopRecorder.getSavedFiles();

                if (recordedFiles.empty())
                {
                    ImGui::Text("No files available.");
                }
                else
                {
                    ImGui::BeginTable("File Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

                    ImGui::TableSetupColumn("Filename", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Play", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    for (const auto& file : recordedFiles)
                    {

                        ImGui::PushID(file.filename.c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        if (ImGui::Selectable(file.filename.c_str()))
                        {
                            std::cout << "Selected: " << file.filename << std::endl;
                        }

                        ImGui::TableSetColumnIndex(1);
                        if (ImGui::Button("Play"))
                        {
                            std::cout << "Play: " << file.filename << std::endl;
                            m_loopPlayer.playAudioFile(file.filename);
                        }

                        ImGui::TableSetColumnIndex(2);
                        if (ImGui::Button("Delete"))
                        {
                            std::cout << "Delete: " << file.filename << std::endl;
                            m_loopRecorder.deleteFile(file);
                        }


                        ImGui::PopID();
                    }

                    // End the table
                    ImGui::EndTable();
                   
                }
            }

        }
        ImGui::End();

    }

}

