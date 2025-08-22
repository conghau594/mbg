// Logger.h
#pragma once

#ifdef _DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace utils
{
  class Logger
  {
  public:
    // TODO: Implement Logger functionality
    static void setUp() noexcept
    {
#ifdef _DEBUG
      spdlog::set_level(spdlog::level::trace);
#else
      spdlog::set_level(spdlog::level::debug);
#endif

      // change log pattern
      // spdlog::set_pattern("[%H:%M:%S] [%t] [%s:%# %!()]\n%^ - [%l] %v%$");
      spdlog::set_pattern("%^[%l] %v%$ [%H:%M:%S] [%t] [%s:%#]");

      // // spdlog usage
      // SPDLOG_INFO("Welcome to spdlog!");

      // SPDLOG_TRACE("Tracing very {}...", "detail");
      // SPDLOG_DEBUG("This message should be displayed..");
      // SPDLOG_INFO("Let's learn the usage of `spdlog`!");
      // SPDLOG_WARN("Easy padding in numbers like {:08d}", 12);
      // SPDLOG_ERROR("Some error message with arg: {}", 1);
      // SPDLOG_CRITICAL("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);

      // SPDLOG_INFO("Support for floats {:03.2f}", 1.23456);
      // SPDLOG_INFO("Positional args are {1} {0}..", "too", "supported");
      // SPDLOG_INFO("{:<30}", "left aligned");
      // SPDLOG_INFO("{:>30}", "right aligned");

      // // Compile time log levels
      // // Note that this does not change the current log level, it will only
      // // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
      // SPDLOG_TRACE("Some trace message with param {}", 42);
      // SPDLOG_DEBUG("Some debug message");
    }
  };
}
