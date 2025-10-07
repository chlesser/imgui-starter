#include "Application.h"
#include "imgui/imgui.h"

namespace ClassGame {
        //
        // our global variables
        //

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            // Initialize logging system
//            Logger& logger = Logger::GetInstance();
//            logger.LogInfo("Game started successfully");
//            logger.LogGameEvent("Application initialized");
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
            //general setup
            ImGui::DockSpaceOverViewport();
            ImGui::ShowDemoWindow();

            // LOG DEMO
            ImGui::Begin("ImGui Log Demo");
            ImGui::LogButtons();

            if (ImGui::Button("Copy \"Hello, world!\" to clipboard"))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("Hello, world!");
                ImGui::LogFinish();
            }
            ImGui::End();

            // GAME LOG
            ImGui::Begin("Game Log");

            //BUTTONS
            if(ImGui::Button("Options"))
                true;
            ImGui::SameLine();
            if(ImGui::Button("Clear"))
                true;
            ImGui::SameLine();
            if(ImGui::Button("Test Info"))
                true;
            ImGui::SameLine();
            if(ImGui::Button("Test Warning"))
                true;
            ImGui::SameLine();
            if(ImGui::Button("Test Error"))
                true;
            ImGui::Separator();
            ImGui::Text("Game Log:");

            
            ImGui::End();
            // GAME CONTROL
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
        }
}
