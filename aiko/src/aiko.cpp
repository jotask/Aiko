#include "aiko.h"

#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>

namespace aiko
{

    void Aiko::run()
    {

        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(800, 450, "raylib [core] example - basic window");

        //  SetTargetFPS(60);

        rlImGuiSetup(true);

        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            rlImGuiBegin();
            bool open = true;
            ImGui::ShowDemoWindow(&open);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(190, 300);
            rlImGuiEnd();
            EndDrawing();
        }

        rlImGuiShutdown();

        CloseWindow();

    }

}
