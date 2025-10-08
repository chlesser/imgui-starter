#include "Application.h"
#include "imgui/imgui.h"
#include <string>
#include <iostream>

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


        enum MessageType {
            INFO,
            WARNING,
            ERROR,
            COMMAND
        }; 

        struct logger { // this code was mostly taken from imgui_demo.cpp.
            ImGuiTextBuffer         buf;
            ImGuiTextFilter         filter;
            ImVector<int>           lineOffsets;
            ImVector<MessageType>   lineTypes;
            MessageType             currentType;
            bool                    autoScroll;
            char                    InputBuf[256];

            logger() {
                autoScroll = true;
                currentType = INFO;
                memset(InputBuf, 0, sizeof(InputBuf));
                Clear();
            }
            void Clear() { //reset
                buf.clear();
                lineOffsets.clear();
                lineOffsets.push_back(0);
                lineTypes.clear();
            }
            static void  Strtrim(char* s) //this is a surprise tool that will help us later
            {
                char* str_end = s + strlen(s);
                while (str_end > s && str_end[-1] == ' ')
                str_end--;
                *str_end = 0;
            } 
            ImVec4 MessageTypeToColor(MessageType type) { //doing this in a seprate function so I can change if I want to easily!
                switch (type) {
                case INFO:
                    return ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                case WARNING:
                    return ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
                case ERROR:
                    return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                default:
                    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
            void AddLog(MessageType type, const char* fmt, ...) IM_FMTARGS(2) {
                int old_size = buf.size();
                va_list args; // variable list of our 
                va_start(args, fmt);
                buf.appendfv(fmt, args);
                va_end(args);
                for (int new_size = buf.size(); old_size < new_size; old_size++)
                    if (buf[old_size] == '\n') {
                        lineOffsets.push_back(old_size + 1);
                        lineTypes.push_back(type);
                    }
            }
            void logError(const char* fmt, ...) IM_FMTARGS(2) {
                std::string stringConvert = fmt;
                stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [ERROR] ");
                stringConvert.append("\n");
                AddLog(ERROR, stringConvert.c_str());
            }
            void logCommand(const char* fmt, ...) IM_FMTARGS(2) {
                std::string stringConvert = fmt;
                stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [COMMAND] ");
                stringConvert.append("\n");
                AddLog(COMMAND, stringConvert.c_str());
            }
            void logWarning(const char* fmt, ...) IM_FMTARGS(2) {
                std::string stringConvert = fmt;
                stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [WARNING] ");
                stringConvert.append("\n");
                AddLog(WARNING, stringConvert.c_str());
            }
            void logInfo(const char* fmt, ...) IM_FMTARGS(2) {
                std::string stringConvert = fmt;
                stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [INFO] ");
                stringConvert.append("\n");
                AddLog(INFO, stringConvert.c_str());
            }
            void Draw(const char* title, bool* p_open = NULL) {
                if (!ImGui::Begin(title, p_open)) {
                    ImGui::End();
                    return;
                }

                // Options menu
                if (ImGui::BeginPopup("Options")) {
                    ImGui::Checkbox("Auto-scroll", &autoScroll);
                    ImGui::EndPopup();
                }

                if(ImGui::Button("Options"))
                    ImGui::OpenPopup("Options");
                ImGui::SameLine();
                if(ImGui::Button("Clear"))
                    Clear();
                ImGui::SameLine();
                if(ImGui::Button("Info"))
                    currentType = INFO;
                ImGui::SameLine();
                if(ImGui::Button("Warning"))
                    currentType = WARNING;
                ImGui::SameLine();
                if(ImGui::Button("Error"))
                    currentType = ERROR;
                ImGui::SameLine();
                filter.Draw("Filter", -100.0f);
                ImGui::Separator();

                const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
                ImGui::BeginChild("scrolling", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

                //text display
                const char* buf_begin = buf.begin();
                const char* buf_end = buf.end();
                for (int line_no = 0; line_no < lineOffsets.Size - 1; line_no++) {
                        const char* line_start = buf_begin + lineOffsets[line_no];
                        const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buf_begin + lineOffsets[line_no + 1] - 1) : buf_end;
                        if(lineTypes[line_no] >= currentType) //first we filter by message type!
                        {
                            if (!filter.IsActive() || filter.PassFilter(line_start, line_end)) { //next we filter by message content!
                                ImGui::PushStyleColor(ImGuiCol_Text, MessageTypeToColor(lineTypes[line_no]));
                                ImGui::TextUnformatted(line_start, line_end);
                                ImGui::PopStyleColor();
                            }
                        }
                    }
                

                if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);

                ImGui::EndChild();
                ImGui::Separator();

                // Command-line
                
                ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
                if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags))
                {
                    char* s = InputBuf;
                    Strtrim(s);
                    if (s[0])
                        ExecCommand(s);
                    strcpy(s, "");
                }

                // Auto-focus on window apparition
                ImGui::SetItemDefaultFocus();

                ImGui::End();
            }
            void    ExecCommand(const char* command_line)
            {
                logCommand(command_line);
            }
        };
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
            static logger game_log;
            ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
            ImGui::Begin("Game Log");

            if (ImGui::SmallButton("Test Warning"))
            {
                game_log.logWarning("Testing a simple Warning Message!");
            }
            if (ImGui::SmallButton("Test Error"))
            {
                game_log.logError("Testing a simple Error Message!");
            }
            if (ImGui::SmallButton("Test Info"))
            {
                game_log.logInfo("Testing a simple Info Message!");
            }
            ImGui::End();
            game_log.Draw("Game Log");

            
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
        }
}
