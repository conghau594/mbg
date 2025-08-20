// EnvUtils.h
#pragma once
#include <fstream>
namespace utils
{
  inline auto parseEnvFile(const std::string &filename)
      -> std::map<std::string, std::string>
  {
    std::map<std::string, std::string> env;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
      // Skip comments and empty lines
      if (line.empty() || line[0] == '#')
        continue;

      std::istringstream iss(line);
      std::string key, value;

      if (std::getline(iss, key, '=') && std::getline(iss, value))
      {
        // Trim whitespace (optional)
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        env[key] = value;
      }
    }
    return env;
  }
} // namespace utils
