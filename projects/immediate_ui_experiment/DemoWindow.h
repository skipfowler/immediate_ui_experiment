#pragma once

#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/StringUtilities.h>

#include "RmlUi_Backend_GLFW_GL3.h"

class DemoWindow : public Rml::EventListener {
 public:
  DemoWindow(const Rml::String &title, Rml::Context *context);

  void Update();
  void Shutdown();
  void ProcessEvent(Rml::Event &event) override;
  Rml::ElementDocument *GetDocument();
  void SubmitForm(Rml::String in_submit_message);
  void SetSandboxStylesheet(const Rml::String &string);
  void SetSandboxBody(const Rml::String &string);
  void SetBackend(Rml::SharedPtr<RmlUi::Backend::glfwGL3> backend);

 private:
  Rml::ElementDocument *document = nullptr;
  Rml::ElementDocument *iframe = nullptr;
  Rml::Element *gauge = nullptr, *progress_horizontal = nullptr;
  Rml::SharedPtr<Rml::StyleSheetContainer> rml_basic_style_sheet;

  bool submitting = false;
  double submitting_start_time = 0;
  Rml::String submit_message;
  Rml::SharedPtr<RmlUi::Backend::glfwGL3> _backend = nullptr;
};