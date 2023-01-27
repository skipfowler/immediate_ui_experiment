#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <memory>

namespace Entities {
class Application {
 public:
  Application(std::shared_ptr<entt::registry> registry);
  ~Application();

 private:
  std::shared_ptr<entt::registry> _registry;
  entt::entity _entity;
};
}  // namespace Entities