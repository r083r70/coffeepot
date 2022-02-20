#pragma once

#include <memory>

#include "spdlog/logger.h"

namespace coffeepot
{
    class log
    {
    public:
        static void init();
        inline static std::shared_ptr<spdlog::logger>& getLogger() { return s_Logger; }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
};

#ifdef DEBUG
#define CP_TRACE(...)   coffeepot::log::getLogger()->trace(__VA_ARGS__)
#define CP_DEBUG(...)   coffeepot::log::getLogger()->debug(__VA_ARGS__)
#define CP_INFO(...)    coffeepot::log::getLogger()->info(__VA_ARGS__)
#define CP_WARN(...)    coffeepot::log::getLogger()->warn(__VA_ARGS__)
#define CP_ERROR(...)   coffeepot::log::getLogger()->error(__VA_ARGS__)
#define CP_FATAL(...)   coffeepot::log::getLogger()->critical(__VA_ARGS__)
#else
#define CP_TRACE(...)
#define CP_DEBUG(...)
#define CP_INFO(...)
#define CP_WARN(...)
#define CP_ERROR(...)
#define CP_FATAL(...)
#endif // DEBUG