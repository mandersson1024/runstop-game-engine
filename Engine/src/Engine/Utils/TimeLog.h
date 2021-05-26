#pragma once
#include <chrono>
#include <string>

namespace Engine
{

    class TimeLog
    {
    public:
        TimeLog(const std::string& text = "");
        ~TimeLog();

        float GetNanoseconds() const;
        float GetMicroseconds() const;
        float GetMilliseconds() const;
        float GetSeconds() const;

    private:
        std::string text_;
        std::chrono::steady_clock::time_point start_;
    };

}
