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
        const auto now = std::chrono::high_resolution_clock::now();

        if (isFirstFrame_)
        {
            isFirstFrame_ = false;
        }
        else
        {
            const auto duration = now - lastFrameTime_;
            deltaTimeSeconds_ = duration.count() * 1e-9f;
        }

        lastFrameTime_ = now;
    }

}
