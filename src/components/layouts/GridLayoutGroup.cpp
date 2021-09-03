#include "shared/components/layouts/GridLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::GridLayoutGroup::render(UnityEngine::Transform *parentTransform) {
    gridLayoutGroup = BeatSaberUI::CreateGridLayoutGroup(parentTransform);

    transform = gridLayoutGroup->get_transform();

    rendered = true;
    update();

    return this;
}