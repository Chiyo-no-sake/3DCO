#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

Log* Log::getInstance()
{
    static Log* instance = nullptr;
    if (instance == nullptr) {
        instance = new Log();
    }

    return instance;
}

Log::Log()
{
    spdlog::set_pattern("%^[%T] %n[%l]: %v%$");
    m_logger = spdlog::stdout_color_mt("3DCO");
    m_logger->set_level(spdlog::level::trace);
}


void Log::setLevel(spdlog::level::level_enum level) {
    m_logger->set_level(level);
}