#pragma once

#include <format>
#include <iostream>
#include <stdexcept>
#include <utility>

template <typename... Args>
void
log(std::format_string<Args...> fmt, Args &&...args) {
  std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

template <typename... Args>
void
warn(std::format_string<Args...> fmt, Args &&...args) {
  std::cerr << "[warn] " << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

template <typename... Args>
[[noreturn]] void
fatal(std::format_string<Args...> fmt, Args &&...args) {
  throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
}