#pragma once
#include <chrono>

namespace Engine
{
    class FrameTimer
    {
    public:
        FrameTimer();

        void Tick();
        float GetDeltaTime() const { return deltaTimeSeconds_; }

    private:
        std::chrono::high_resolution_clock clock_;
        std::chrono::steady_clock::time_point lastFrameTime_;
        bool isFirstFrame_ = true;
        float deltaTimeSeconds_ = 0;
    };

}

