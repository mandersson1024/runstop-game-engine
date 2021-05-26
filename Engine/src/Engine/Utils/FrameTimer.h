#pragma once
#include <chrono>

namespace Engine
{
    class FrameTimer
    {
    public:
        FrameTimer();

        float Tick();

        float GetDeltaTime() const
        {
            return deltaTimeSeconds_;
        }

    private:
        static float constexpr nanosecondsPerSecond_ = 1000 * 1000 * 1000;
        std::chrono::high_resolution_clock clock_;
        std::chrono::steady_clock::time_point lastFrameTime_;
        bool isFirstFrame_;
        float deltaTimeSeconds_;
    };

}

