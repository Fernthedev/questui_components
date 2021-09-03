#include "shared/components/ScrollableContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::ScrollableContainer::render(UnityEngine::Transform *parentTransform) {
    UnityEngine::GameObject* scrollableContainer = BeatSaberUI::CreateScrollableSettingsContainer(parentTransform);

    transform = scrollableContainer->get_transform();

    rendered = true;
    update();

    return this;
}