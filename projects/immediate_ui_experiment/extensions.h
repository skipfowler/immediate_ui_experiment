#pragma once

#include <string>
#include <vector>

namespace Fillet::Extensions {
constexpr std::vector<std::string_view> cpp() noexcept {
  /**
  */
  return std::vector<std::string_view>{".cc", ".cpp", ".h", ".hpp"};
}

constexpr std::vector<std::string_view> json() noexcept {
  return std::vector<std::string_view>{".json"};
}

constexpr std::vector<std::string_view> lua() noexcept {
  return std::vector<std::string_view>{".lua"};
}

constexpr std::vector<std::string_view> py() noexcept {
  return std::vector<std::string_view>{".py"};
}

constexpr std::vector<std::string_view> xml() noexcept {
  return std::vector<std::string_view>{".xml"};
}
}