#ifndef ULITILITIES_HPP__
#define ULITILITIES_HPP__
#include <fmt/format.h>
#define LOG_INFO(f, args...) \
  fmt::print("{}:{}{}: " f "\n", __FILE__, __FUNCTION__, __LINE__, ##args)
#define lengthof(A) ((sizeof(A) / sizeof((A)[0])))
#endif