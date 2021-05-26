#include "pch.h"
#include "Logger.h"
#include "spdlog/spdlog.h"
#include "Platform/ConsoleWindow.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Engine
{

    std::shared_ptr<spdlog::logger> Logger::logger_;
    std::mutex Logger::mutex_;

    void Engine::Logger::Init()
    {
        std::vector<spdlog::sink_ptr> sinks;

        if (ConsoleWindow::IsCreated())
        {
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            consoleSink->set_color(spdlog::level::warn, FOREGROUND_RED | FOREGROUND_GREEN);
            consoleSink->set_color(spdlog::level::err, FOREGROUND_RED);
            consoleSink->set_pattern("%^%v%$");
            sinks.push_back(consoleSink);
        }

        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("engine_log.txt", true);
        fileSink->set_pattern("[%T] %-7l - %v");
        sinks.push_back(fileSink);

        {
            const std::lock_guard lock(mutex_);
            logger_ = std::make_shared<spdlog::logger>("engine", begin(sinks), end(sinks));
            logger_->set_level(spdlog::level::trace);
            logger_->flush_on(spdlog::level::trace);
        }

        /*
        ENGINE_LOG("ENGINE_LOG");
        ENGINE_LOG_INFO("ENGINE_LOG_INFO");
        ENGINE_LOG_WARNING("ENGINE_LOG_WARNING");
        ENGINE_LOG_ERROR("ENGINE_LOG_ERROR");
        ENGINE_LOG_DEBUG("ENGINE_LOG_DEBUG");
        ENGINE_LOG_DEBUG_WARNING("ENGINE_LOG_DEBUG_WARNING");
        ENGINE_LOG_DEBUG_ERROR("ENGINE_LOG_DEBUG_ERROR");
        */

        ENGINE_LOG("Logger initialized");
    }

    void Logger::Shutdown()
    {
        std::lock_guard lock(mutex_);
        logger_ = nullptr;
    }

}
