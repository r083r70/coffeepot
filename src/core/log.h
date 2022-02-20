#pragma once

#include <memory>

#include "spdlog/logger.h"

namespace coffeepot
{
    class Log
    {
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
};

#ifdef DEBUG
#define CP_TRACE(...)   coffeepot::Log::GetLogger()->trace(__VA_ARGS__)
#define CP_DEBUG(...)   coffeepot::Log::GetLogger()->debug(__VA_ARGS__)
#define CP_INFO(...)    coffeepot::Log::GetLogger()->info(__VA_ARGS__)
#define CP_WARN(...)    coffeepot::Log::GetLogger()->warn(__VA_ARGS__)
#define CP_ERROR(...)   coffeepot::Log::GetLogger()->error(__VA_ARGS__)
#define CP_FATAL(...)   coffeepot::Log::GetLogger()->critical(__VA_ARGS__)
#else
#define CP_TRACE(...)
#define CP_DEBUG(...)
#define CP_INFO(...)
#define CP_WARN(...)
#define CP_ERROR(...)
#define CP_FATAL(...)
#endif // DEBUG