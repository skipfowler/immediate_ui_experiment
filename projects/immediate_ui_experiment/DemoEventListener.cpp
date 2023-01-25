#include "DemoEventListener.h"

#include <RmlUi/Core/Elements/ElementFormControl.h>

#include "RmlUi_Backend_GLFW_GL3.h"

struct TweeningParameters {
  Rml::Tween::Type type = Rml::Tween::Linear;
  Rml::Tween::Direction direction = Rml::Tween::Out;
  float duration = 0.5f;
} tweening_parameters;

DemoEventListener::DemoEventListener(const Rml::String& value,
                                     Rml::Element* element)
    : _value(value), _element(element) {}

void DemoEventListener::ProcessEvent(Rml::Event& event) {
  using namespace Rml;

  if (_value == "exit") {
    // Test replacing the current element.
    // Need to be careful with regard to lifetime issues. The event's current
    // element will be destroyed, so we cannot use it after SetInnerRml(). The
    // library should handle this case safely internally when propagating the
    // event further.
    Element* parent = _element->GetParentNode();
    parent->SetInnerRML(
        "<button onclick='confirm_exit' onblur='cancel_exit' "
        "onmouseout='cancel_exit'>Are you sure?</button>");
    if (Element* child = parent->GetChild(0)) child->Focus();
  } else if (_value == "confirm_exit") {
    _backend->RequestExit();
  } else if (_value == "cancel_exit") {
    if (Element* parent = _element->GetParentNode())
      parent->SetInnerRML("<button id='exit' onclick='exit'>Exit</button>");
  } else if (_value == "change_color") {
    Colourb color((byte)Math::RandomInteger(255),
                  (byte)Math::RandomInteger(255),
                  (byte)Math::RandomInteger(255));
    _element->Animate(
        "image-color", Property(color, Property::COLOUR),
        tweening_parameters.duration,
        Tween(tweening_parameters.type, tweening_parameters.direction));
    event.StopPropagation();
  } else if (_value == "move_child") {
    Vector2f mouse_pos(event.GetParameter("mouse_x", 0.0f),
                       event.GetParameter("mouse_y", 0.0f));
    if (Element* child = _element->GetFirstChild()) {
      Vector2f new_pos = mouse_pos - _element->GetAbsoluteOffset() -
                         Vector2f(0.35f * child->GetClientWidth(),
                                  0.9f * child->GetClientHeight());
      Property destination = Transform::MakeProperty(
          {Transforms::Translate2D(new_pos.x, new_pos.y)});
      if (tweening_parameters.duration <= 0)
        child->SetProperty(PropertyId::Transform, destination);
      else
        child->Animate(
            "transform", destination, tweening_parameters.duration,
            Tween(tweening_parameters.type, tweening_parameters.direction));
    }
  } else if (_value == "tween_function") {
    static const SmallUnorderedMap<String, Tween::Type> tweening_functions = {
        {"back", Tween::Back},         {"bounce", Tween::Bounce},
        {"circular", Tween::Circular}, {"cubic", Tween::Cubic},
        {"elastic", Tween::Elastic},   {"exponential", Tween::Exponential},
        {"linear", Tween::Linear},     {"quadratic", Tween::Quadratic},
        {"quartic", Tween::Quartic},   {"quintic", Tween::Quintic},
        {"sine", Tween::Sine}};

    String value = event.GetParameter("value", String());
    auto it = tweening_functions.find(value);
    if (it != tweening_functions.end())
      tweening_parameters.type = it->second;
    else {
      RMLUI_ERROR;
    }
  } else if (_value == "tween_direction") {
    String value = event.GetParameter("value", String());
    if (value == "in")
      tweening_parameters.direction = Tween::In;
    else if (value == "out")
      tweening_parameters.direction = Tween::Out;
    else if (value == "in-out")
      tweening_parameters.direction = Tween::InOut;
    else {
      RMLUI_ERROR;
    }
  } else if (_value == "tween_duration") {
    float value = (float)std::atof(
        static_cast<Rml::ElementFormControl*>(_element)->GetValue().c_str());
    tweening_parameters.duration = value;
    if (auto el_duration = _element->GetElementById("duration"))
      el_duration->SetInnerRML(CreateString(20, "%2.2f", value));
  } else if (_value == "rating") {
    auto el_rating = _element->GetElementById("rating");
    auto el_rating_emoji = _element->GetElementById("rating_emoji");
    if (el_rating && el_rating_emoji) {
      enum { Sad, Mediocre, Exciting, Celebrate, Champion, CountEmojis };
      static const Rml::String emojis[CountEmojis] = {
          (const char*)u8"😢", (const char*)u8"😐", (const char*)u8"😮",
          (const char*)u8"😎", (const char*)u8"🏆"};
      int value = event.GetParameter("value", 50);

      Rml::String emoji;
      if (value <= 0)
        emoji = emojis[Sad];
      else if (value < 50)
        emoji = emojis[Mediocre];
      else if (value < 75)
        emoji = emojis[Exciting];
      else if (value < 100)
        emoji = emojis[Celebrate];
      else
        emoji = emojis[Champion];

      el_rating->SetInnerRML(Rml::CreateString(30, "%d%%", value));
      el_rating_emoji->SetInnerRML(emoji);
    }
  } else if (_value == "submit_form") {
    const auto& p = event.GetParameters();
    Rml::String output = "<p>";
    for (auto& entry : p) {
      auto value =
          Rml::StringUtilities::EncodeRml(entry.second.Get<Rml::String>());
      if (entry.first == "message") value = "<br/>" + value;
      output += "<strong>" + entry.first + "</strong>: " + value + "<br/>";
    }
    output += "</p>";

    _window->SubmitForm(output);
  } else if (_value == "set_sandbox_body") {
    if (auto source = static_cast<Rml::ElementFormControl*>(
            _element->GetElementById("sandbox_rml_source"))) {
      auto value = source->GetValue();
      _window->SetSandboxBody(value);
    }
  } else if (_value == "set_sandbox_style") {
    if (auto source = static_cast<Rml::ElementFormControl*>(
            _element->GetElementById("sandbox_rcss_source"))) {
      auto value = source->GetValue();
      _window->SetSandboxStylesheet(value);
    }
  }
}

void DemoEventListener::OnDetach(Rml::Element* /*element*/) { delete this; }

void DemoEventListener::SetBackend(Rml::SharedPtr<RmlUi::Backend::glfwGL3> backend) {
  _backend = backend;
}
void DemoEventListener::SetWindow(Rml::SharedPtr<DemoWindow> window) {
  _window = window;
}