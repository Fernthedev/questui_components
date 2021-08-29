#include "components/HoverHint.hpp"

#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::HoverHint::render(UnityEngine::Transform *parentTransform) {
    transform = BeatSaberUI::AddHoverHint(renderComponent(child, parentTransform)->getTransform()->get_gameObject(), std::string(text))->get_transform();
    return this;
}
