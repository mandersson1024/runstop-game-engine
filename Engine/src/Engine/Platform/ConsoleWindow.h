#pragma once
#include <windows.h>

namespace Engine
{

    class ConsoleWindow
    {
    public:
        ConsoleWindow() = delete;

        static void Create();
        static void Destroy();
        static bool IsCreated() { return created_; }

    private:
        static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

    private:
        static bool created_;
    };

}

