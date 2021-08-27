#include "components/HoverHint.hpp"

#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;

UnityEngine::Transform* QuestUI_Components::HoverHint::render(UnityEngine::Transform *parentTransform) {
    return transform = BeatSaberUI::AddHoverHint(renderComponent(child, parentTransform)->get_gameObject(), std::string(text))->get_transform();
}
