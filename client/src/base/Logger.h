// Logger.h
#pragma once

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
      spdlog::set_level(spdlog::level::warn);
#endif

      // change log pattern
      spdlog::set_pattern("[%H:%M:%S] [%t] [-%L-] %^%v%$");

      // spdlog usage
      spdlog::info("Welcome to spdlog!");

      spdlog::trace("Tracing very {}...", "detail");
      spdlog::debug("This message should be displayed..");
      spdlog::info("Let's learn the usage of `spdlog`!");
      spdlog::warn("Easy padding in numbers like {:08d}", 12);
      spdlog::error("Some error message with arg: {}", 1);
      spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);

      spdlog::info("Support for floats {:03.2f}", 1.23456);
      spdlog::info("Positional args are {1} {0}..", "too", "supported");
      spdlog::info("{:<30}", "left aligned");
      spdlog::info("{:>30}", "right aligned");

      // Compile time log levels
      // Note that this does not change the current log level, it will only
      // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
      SPDLOG_TRACE("Some trace message with param {}", 42);
      SPDLOG_DEBUG("Some debug message");
    }
  };
}
