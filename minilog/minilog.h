#pragma once

#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <source_location>
#include <chrono>
#include <fstream>
#include <string_view>
#include <sys/types.h>

namespace minilog {
#define FOREACH_LOG_LEVEL(f) \
  f(trace) \
  f(debug) \
  f(info) \
  f(warning) \
  f(error) \
  f(fatal)

enum class LogLevel: uint8_t {
#define _FUNCTION(name) name,
  FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
};

namespace details {
template <typename T>
struct with_source_location {
private:
  T inner;
  std::source_location loc;
public:
  template<typename U>
    requires std::constructible_from<T, U> 
    consteval with_source_location(U &&_inner, std::source_location _loc = std::source_location::current())
      : inner{std::forward<U>(_inner)}, loc{std::move(_loc)} {} 
    
    constexpr auto format() const noexcept -> const T & { return inner; }
    constexpr auto location() const noexcept -> const std::source_location & { return loc; }
};

inline std::string log_level_name(LogLevel lev) {
  switch (lev) {
#define _FUNCTION(name) case LogLevel::name: return #name;
  FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
  }
  return "unknown";
}

inline LogLevel log_level_from_name(std::string lev) {
  #define _FUNCTION(name) if (lev == #name) return LogLevel::name;
    FOREACH_LOG_LEVEL(_FUNCTION)
  #undef _FUNCTION
  return LogLevel::info;
}

inline LogLevel g_max_level = []() -> LogLevel {
  if (auto lev = std::getenv("MINILOG_LEVEL")) {
    return log_level_from_name(lev);
  }
  return LogLevel::info;
}();

inline std::fstream g_log_file = []() -> std::fstream {
  if (auto file_path = std::getenv("MINILOG_FILE")) {
    return std::fstream(file_path, std::ios::app);
  }
  return std::fstream{};
}();

#if defined(__linux__) || defined (__APPLE__) 
inline constexpr char k_level_ansi_colors[(uint8_t)LogLevel::fatal + 1][8] = {
  "\E[37m",
  "\E[35m",
  "\E[32m",
  "\E[34m",
  "\E[33m",
  "\E[31m",
};
inline constexpr char k_reset_ansi_color[4] = "\E[m";
#define K_ANSI_COLORS(x) x
#else
inline constexpr char k_level_ansi_colors[(uint8_t)LogLogLevel::fatal + 1)][1] = {
  "",
  "",
  "",
  "",
  "",
  "",
}
inline constexpr char k_reset_ansi_color[1] = ""; 
#define KANSK_ANSI_COLORS(x)
#endif

inline void output_log(LogLevel level, std::string msg, const std::source_location &loc) {
  // output format
  std::chrono::zoned_time now{std::chrono::current_zone(), std::chrono::system_clock::now()};
  auto level_name = log_level_name(level);
  level_name = K_ANSI_COLORS(k_level_ansi_colors[(uint8_t)level]) + level_name + K_ANSI_COLORS(k_reset_ansi_color);
  msg = std::format("[{}] {} {} [{}]: {}", now, loc.file_name(), loc.line(), level_name, msg);
  if (level >= g_max_level) {
    std::cout << msg + '\n';
  }
  if (g_log_file.good()) {
    g_log_file << msg + '\n';
  }
} 

template<typename... Args>
void generic_log(LogLevel lev, with_source_location<std::format_string<Args...>> fmt, Args&&... args) {
  const auto &loc = fmt.location();
  auto msg = std::vformat(fmt.format().get(), std::make_format_args(args...));
  output_log(lev, msg, loc);
}
} // namesapce details

inline void set_log_level(LogLevel lev) {
  details::g_max_level = lev;
}

inline void set_log_file(const std::string &file_path) {
  details::g_log_file = std::fstream(file_path, std::ios::app);
}

#define _FUNCTION(name) template<typename... Args> \
void log_##name(details::with_source_location<std::format_string<Args...>> fmt, Args&&... args) { \
  generic_log(LogLevel::name, std::move(fmt), std::forward<Args...>(args...)); \
} 
FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION 

}  // namesapce minidog






