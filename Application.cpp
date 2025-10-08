#include "Application.h"
#include "imgui/imgui.h"
#include <string>

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

        struct logger {
            ImGuiTextBuffer         buf;
            ImGuiTextFilter         filter;
            ImVector<int>           lineOffsets;
            ImVector<MessageType>   lineTypes;
            bool                    currentTypes[4]; //Monitors which message types are currently enabled.
            bool                    autoScroll;
            char                    InputBuf[256];

            logger() {
                autoScroll = true;
                memset(InputBuf, 0, sizeof(InputBuf));
                EnableCurrentTypes();
                Clear();
            }
            void EnableCurrentTypes() { currentTypes[INFO] = true; currentTypes[WARNING] = true; currentTypes[ERROR] = true; currentTypes[COMMAND] = true; }

            //clear resets the log.
            void Clear() { buf.clear(); lineOffsets.clear(); lineOffsets.push_back(0); lineTypes.clear(); }

            //simply cuts off whitespace, taken from logger demo.
            static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; } 

            //maps message types to colors. I will collapse once I like the colors.
            ImVec4 MessageTypeToColor(MessageType type) {
                switch (type) {
                    case INFO: return ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                    case WARNING: return ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
                    case ERROR: return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }

            //AddLog is taken from the demo class for the most part. I added a messageType parameter.
            void AddLog(MessageType type, const char* fmt, ...) IM_FMTARGS(2) {
                int old_size = buf.size();
                va_list args;
                va_start(args, fmt);
                buf.appendfv(fmt, args);
                va_end(args);
                for (int new_size = buf.size(); old_size < new_size; old_size++)
                    if (buf[old_size] == '\n') {
                        lineOffsets.push_back(old_size + 1);
                        lineTypes.push_back(type);
                    }
            }

            // LogError, LogWarning, LogInfo, and LogCommand are convenience functions to log messages of different types.
            //They add a timestamp and the message type to the start of the message, as well as a newline to the end.
            void LogError(const char* fmt, ...) { std::string stringConvert = fmt; stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [ERROR] "); stringConvert.append("\n"); AddLog(ERROR, stringConvert.c_str()); }
            void LogCommand(const char* fmt, ...) { std::string stringConvert = fmt; stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [COMMAND] "); stringConvert.append("\n"); AddLog(COMMAND, stringConvert.c_str()); }
            void LogWarning(const char* fmt, ...) { std::string stringConvert = fmt; stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [WARNING] "); stringConvert.append("\n"); AddLog(WARNING, stringConvert.c_str()); }
            void LogInfo(const char* fmt, ...) { std::string stringConvert = fmt; stringConvert.insert(0, "[" + std::to_string(ImGui::GetTime()) + "] [INFO] "); stringConvert.append("\n"); AddLog(INFO, stringConvert.c_str());}
            
            void Draw(const char* title, bool* p_open = NULL) {
                if (!ImGui::Begin(title, p_open)) { //Singleton!
                    ImGui::End();
                    return;
                }
                
                //Output Buttons
                if(ImGui::Button("Log To Console"))
                    ImGui::LogToTTY();
                ImGui::SameLine();
                if(ImGui::Button("Log To File"))
                    ImGui::LogToFile();
                ImGui::SameLine();
                if(ImGui::Button("Log To Clipboard"))
                    ImGui::LogToClipboard();
                ImGui::Separator();

                //Popups
                if (ImGui::BeginPopup("Options")) {
                    ImGui::Checkbox("Auto-scroll", &autoScroll);
                    ImGui::EndPopup();
                }
                if (ImGui::BeginPopup("Message Types")) {
                    ImGui::Checkbox("Info", &currentTypes[INFO]);
                    ImGui::Checkbox("Warning", &currentTypes[WARNING]);
                    ImGui::Checkbox("Error", &currentTypes[ERROR]);
                    ImGui::EndPopup();
                }

                //Buttons
                if(ImGui::Button("Options"))
                    ImGui::OpenPopup("Options");
                ImGui::SameLine();
                if(ImGui::Button("Message Types"))
                    ImGui::OpenPopup("Message Types");
                ImGui::SameLine();
                if(ImGui::Button("Clear"))
                    Clear();
                ImGui::SameLine();
                filter.Draw("Filter", -100.0f);
                ImGui::Separator();

                //Scrolling Text
                //This was mostly taken from imgui_demo.cpp. I reordered the filtering by text, and added filtering by message type. Coloring as well.
                const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
                ImGui::BeginChild("scrolling", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

                const char* buf_begin = buf.begin();
                const char* buf_end = buf.end();
                for (int line_no = 0; line_no < lineOffsets.Size - 1; line_no++) {
                        const char* line_start = buf_begin + lineOffsets[line_no];
                        const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buf_begin + lineOffsets[line_no + 1] - 1) : buf_end;
                        if(currentTypes[lineTypes[line_no]]) //first we filter by message type!
                        {
                            if (!filter.IsActive() || filter.PassFilter(line_start, line_end)) { //next we filter by message content!
                                ImGui::PushStyleColor(ImGuiCol_Text, MessageTypeToColor(lineTypes[line_no]));
                                ImGui::TextUnformatted(line_start, line_end);
                                ImGui::PopStyleColor();
                            }
                        }
                    }
                //auto-scroll from imgui_demo.cpp
                if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);

                ImGui::EndChild();
                ImGui::Separator();

                // Command-line
                // Code partially taken from imgui_demo.cpp, but heavily modified.
                ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
                if (ImGui::InputText("Console", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags))
                {
                    char* s = InputBuf;
                    Strtrim(s);
                    if (s[0])
                        ExecCommand(s);
                    strcpy(s, "");
                }

                ImGui::SetItemDefaultFocus(); //In all honesty, the demo code had this but I don't know what it does.

                ImGui::End();
            }
            void    ExecCommand(const char* command_line)
            {
                LogCommand(command_line); //I realize there is a vulnerability in that if I pass %<something> the log will break.

                //here we will do strcmp to known commands and execute them.
            }
        };
        void RenderGame() 
        {
            //general setup
            ImGui::DockSpaceOverViewport();

            // GAME LOG
            static logger game_log;
            ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
            ImGui::Begin("Game Log");
            ImGui::End();
            game_log.Draw("Game Log");

            //TEST WINDOW
            ImGui::Begin("Tester");
            if (ImGui::SmallButton("Test Warning"))
            {
                game_log.LogWarning("Testing a simple Warning Message!");
            }
            if (ImGui::SmallButton("Test Error"))
            {
                game_log.LogError("Testing a simple Error Message!");
            }
            if (ImGui::SmallButton("Test Info"))
            {
                game_log.LogInfo("Testing a simple Info Message!");
            }
            ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
        }
}
