#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "glm/glm.hpp"
#include "iomanip"

class Log
{
public:
    static Log* getInstance();

    inline std::shared_ptr<spdlog::logger> getLogger() {
        return m_logger;
    };

    void setLevel(spdlog::level::level_enum level);

private:
    Log();
    std::shared_ptr<spdlog::logger> m_logger;
};

#define CO_LOG_TRACE(...) Log::getInstance()->getLogger()->trace(__VA_ARGS__)
#define CO_LOG_INFO(...) Log::getInstance()->getLogger()->info(__VA_ARGS__)
#define CO_LOG_WARN(...) Log::getInstance()->getLogger()->warn(__VA_ARGS__)
#define CO_LOG_ERR(...) Log::getInstance()->getLogger()->error(__VA_ARGS__)


