#include "pch.h"
#include "ConsoleWindow.h"
#include "Engine/Logger.h"

namespace Engine
{
    bool ConsoleWindow::created_ = false;

    void ConsoleWindow::Create()
    {
        ENGINE_LOG("Creating ConsoleWindow");

        AllocConsole();

        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

        SetConsoleCtrlHandler(CtrlHandler, TRUE);

        created_ = true;
    }

    void ConsoleWindow::Destroy()
    {
        ENGINE_LOG("Destroying ConsoleWindow");

        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
    }

    BOOL WINAPI ConsoleWindow::CtrlHandler(DWORD fdwCtrlType)
    {
        switch (fdwCtrlType)
        {
            // Override the default close event and do nothing.
            // Note that the application may still exit upon closing
            // the console window, but that may be as a result of
            // handling WM_CLOSE in WndProc.

            case CTRL_CLOSE_EVENT:
                return TRUE;

            default:
                return FALSE;
        }
    }

}

