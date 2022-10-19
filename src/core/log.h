#pragma once

#include <memory>

#include "spdlog/logger.h"

namespace coffeepot
{
    class Log
    {
    public:
        static void init();
        inline static std::shared_ptr<spdlog::logger>& getLogger() { return s_Logger; }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
};

#if DEBUG
#define CP_TRACE(...)   coffeepot::Log::getLogger()->trace(__VA_ARGS__)
#define CP_DEBUG(...)   coffeepot::Log::getLogger()->debug(__VA_ARGS__)
#define CP_INFO(...)    coffeepot::Log::getLogger()->info(__VA_ARGS__)
#define CP_WARN(...)    coffeepot::Log::getLogger()->warn(__VA_ARGS__)
#define CP_ERROR(...)   coffeepot::Log::getLogger()->error(__VA_ARGS__)
#define CP_FATAL(...)   coffeepot::Log::getLogger()->critical(__VA_ARGS__)
#else
#define CP_TRACE(...)
#define CP_DEBUG(...)
#define CP_INFO(...)
#define CP_WARN(...)
#define CP_ERROR(...)
#define CP_FATAL(...)
#endif // DEBUG