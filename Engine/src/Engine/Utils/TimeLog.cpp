#include "pch.h"
#include "TimeLog.h"

namespace Engine
{

    TimeLog::TimeLog(const std::string& text) : start_(std::chrono::high_resolution_clock::now())
        , text_(text)
    {
        //ENGINE_LOG("[ScopedTimer {}]", text_);
    }

    TimeLog::~TimeLog()
    {
        ENGINE_LOG("[TimeLog {} ms] {}", GetMilliseconds(), text_);
    }

    float TimeLog::GetNanoseconds() const
    {
        return static_cast<float>((std::chrono::high_resolution_clock::now() - start_).count());
    }

    float TimeLog::GetMicroseconds() const
    {
        return GetNanoseconds() * 1e-3f;
    }

    float TimeLog::GetMilliseconds() const
    {
        return GetNanoseconds() * 1e-6f;
    }

    float TimeLog::GetSeconds() const
    {
        return GetNanoseconds() * 1e-9f;
    }

}

