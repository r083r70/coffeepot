
#include "log.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace coffeepot
{
    std::shared_ptr<spdlog::logger> log::s_Logger;

    void log::init()
    {
        spdlog::set_pattern("%^%T - [%l]: %v%$");
        s_Logger = spdlog::stdout_color_mt("console");
        s_Logger->set_level(spdlog::level::trace);
    }
};