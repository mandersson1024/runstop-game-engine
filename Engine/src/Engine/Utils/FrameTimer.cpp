#include "pch.h"
#include "FrameTimer.h"

namespace Engine
{

    FrameTimer::FrameTimer() : clock_{}
        , isFirstFrame_(true)
        , lastFrameTime_(std::chrono::steady_clock::duration::zero())
        , deltaTimeSeconds_(0)
    {
    }

    float FrameTimer::Tick()
    {
        auto now = std::chrono::high_resolution_clock::now();

        if (isFirstFrame_)
        {
            deltaTimeSeconds_ = 0.f;
            isFirstFrame_ = false;
            lastFrameTime_ = now;
        }
        else
        {
            auto duration = now - lastFrameTime_;
            deltaTimeSeconds_ = duration.count() / nanosecondsPerSecond_;
            lastFrameTime_ = now;
        }

        return deltaTimeSeconds_;
    }

}
