# ImGui Logger
Charles Lesser

This logger was made in C++ using the Dear ImGui library. It is entirely contained within Application.cpp as a struct.
It was created for the CMPM123 class at UC Santa Cruz, using a framework from THE Graeme Devine.

## Features
This assignment initially details 4 requirements aside from modification of the README, but I added in additional features that I felt would be helpful. They are as follows:
- [REQUIRED] Logs to console
- [REQUIRED] Logs to file
- [REQUIRED] Levels of Logging (Info, Warning, Error, Etc)
- [REQUIRED] Can filter by logging level
- Filters by text input
- Console for entry of commands
- Logs to clipboard

## Method
Learning ImGui was difficult, as I could not access the documentation on the wiki page. I designed my program with the following methods:
- Investigating the examples in imgui_demo.cpp, and utilizing these functions when able (I commented these instances when possible).
- Investigating standard functions in imgui.cpp
- Using the C++ standard library for structures like va_list.
- Researching on stack overflow (largely unhelpful, but interesting).
- Debugging with friends (I did this was with the console output).