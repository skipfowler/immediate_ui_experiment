#pragma once

#include <string>

namespace Components {
struct ApplicationError {
  std::string description;
  int error_code;
};
}  // namespace Components