#include "pch.h"
#include "FrameTimer.h"

namespace Engine
{

    FrameTimer::FrameTimer() 
        : clock_{}
    {
    }

    void FrameTimer::Tick()
    {
        auto now = std::chrono::high_resolution_clock::now();

        if (isFirstFrame_)
        {
            isFirstFrame_ = false;
        }
        else
        {
            auto duration = now - lastFrameTime_;
            deltaTimeSeconds_ = duration.count() / 1e9f;
        }

        lastFrameTime_ = now;
    }

}
