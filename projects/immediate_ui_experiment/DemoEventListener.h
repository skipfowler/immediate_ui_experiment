#pragma once

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/StringUtilities.h>

#include "DemoWindow.h"
#include "RmlUi_Backend_GLFW_GL3.h"

class DemoEventListener : public Rml::EventListener {
 public:
  DemoEventListener(const Rml::String& value, Rml::Element* element);
  void ProcessEvent(Rml::Event& event) override;
  void OnDetach(Rml::Element* element) override;
  void SetBackend(Rml::SharedPtr<RmlUi::Backend::glfwGL3> backend);
  void SetWindow(Rml::SharedPtr<DemoWindow> window);

 private:
  Rml::String _value;
  Rml::Element* _element;
  Rml::SharedPtr<DemoWindow> _window;
  Rml::SharedPtr<RmlUi::Backend::glfwGL3> _backend;
};