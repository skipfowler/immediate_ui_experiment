#include "DemoEventListenerInstancer.h"

#include "DemoEventListener.h"

Rml::EventListener* DemoEventListenerInstancer::InstanceEventListener(
    const Rml::String& value, Rml::Element* element) {
  auto event_listener = new DemoEventListener(value, element);
  event_listener->SetBackend(_backend);
  event_listener->SetWindow(_window);
  return event_listener;
}

void DemoEventListenerInstancer::SetBackend(
    Rml::SharedPtr<RmlUi::Backend::glfwGL3> backend) {
  _backend = backend;
}
void DemoEventListenerInstancer::SetWindow(Rml::SharedPtr<DemoWindow> window) {
  _window = window;
}