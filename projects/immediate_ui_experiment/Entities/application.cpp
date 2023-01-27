#pragma once

#include "application.h"

#include "Tags/application_tag.h"

namespace Entities {
Application::Application(std::shared_ptr<entt::registry> registry) : _registry(registry) {
  _entity = registry->create();
  registry->emplace<Tags::ApplicationTag>(_entity);
}

Application::~Application() { _registry->destroy(_entity); }
}  // namespace Entities