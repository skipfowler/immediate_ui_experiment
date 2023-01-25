#pragma once

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/EventListenerInstancer.h>
#include <RmlUi/Core/StringUtilities.h>

#include "DemoWindow.h"
#include "RmlUi_Backend_GLFW_GL3.h"

class DemoEventListenerInstancer : public Rml::EventListenerInstancer {
 public:
  Rml::EventListener* InstanceEventListener(const Rml::String& value,
                                            Rml::Element* element) override;

  void SetBackend(Rml::SharedPtr<RmlUi::Backend::glfwGL3> backend);
  void SetWindow(Rml::SharedPtr<DemoWindow> window);

 private:
  Rml::SharedPtr<RmlUi::Backend::glfwGL3> _backend = nullptr;
  Rml::SharedPtr<DemoWindow> _window;
};