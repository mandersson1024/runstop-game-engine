#pragma once
#include <mutex>
#include "spdlog\logger.h"

namespace Engine
{

    class Logger
    {

    public:
        static void Init();
        static void Shutdown();

        template<typename ... Args>
        static void Trace(Args&& ... args) 
        { 
            std::lock_guard lock(mutex_);
            if (logger_)
            {
                logger_->trace(std::forward<Args>(args)...);
            }
        }

        template<typename ... Args>
        static void Info(Args&& ... args)
        {
            std::lock_guard lock(mutex_);
            if (logger_)
            {
                logger_->info(std::forward<Args>(args)...);
            }
        }

        template<typename ... Args>
        static void Warn(Args&& ... args)
        {
            std::lock_guard lock(mutex_);
            if (logger_)
            {
                logger_->warn(std::forward<Args>(args)...);
            }
        }

        template<typename ... Args>
        static void Error(Args&& ... args)
        {
            std::lock_guard lock(mutex_);
            if (logger_)
            {
                logger_->error(std::forward<Args>(args)...);
            }
        }

        template<typename ... Args>
        static void Debug(Args&& ... args)
        {
            std::lock_guard lock(mutex_);
            if (logger_)
            {
                logger_->debug(std::forward<Args>(args)...);
            }
        }

    private:
        static std::shared_ptr<spdlog::logger> logger_;
        static std::mutex mutex_;
    };

}

#define ENGINE_LOG(...) ::Engine::Logger::Trace(__VA_ARGS__)
#define ENGINE_LOG_INFO(...) ::Engine::Logger::Info(__VA_ARGS__)
#define ENGINE_LOG_WARNING(...) ::Engine::Logger::Warn(__VA_ARGS__)
#define ENGINE_LOG_ERROR(...) ::Engine::Logger::Error(__VA_ARGS__)

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define ENGINE_LOG_DEBUG(...) ::Engine::Logger::Debug("[{}:{}, {}] {}", __FILENAME__, __LINE__, __FUNCTION__, std::format(__VA_ARGS__))
#define ENGINE_LOG_DEBUG_WARNING(...) ::Engine::Logger::Warn("[{}:{}, {}] {}", __FILENAME__, __LINE__, __FUNCTION__, std::format(__VA_ARGS__))
#define ENGINE_LOG_DEBUG_ERROR(...) ::Engine::Logger::Error("[{}:{}, {}] {}", __FILENAME__, __LINE__, __FUNCTION__, std::format(__VA_ARGS__))

