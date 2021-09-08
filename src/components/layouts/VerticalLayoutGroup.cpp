#include "shared/components/layouts/VerticalLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::VerticalLayoutGroup::render(UnityEngine::Transform *parentTransform) {
    verticalLayoutGroup = BeatSaberUI::CreateVerticalLayoutGroup(parentTransform);

    transform = verticalLayoutGroup->get_transform();

    rendered = true;
    update();

    return this;
}