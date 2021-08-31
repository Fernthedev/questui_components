#include "components/layouts/HorizontalLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::HorizontalLayoutGroup::render(UnityEngine::Transform *parentTransform) {
    horizontalLayoutGroup = BeatSaberUI::CreateHorizontalLayoutGroup(parentTransform);

    transform = horizontalLayoutGroup->get_transform();

    rendered = true;
    update();

    return this;
}