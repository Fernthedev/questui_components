#include "shared/components/layouts/MultiComponentGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::MultiComponentGroup::render(UnityEngine::Transform *parentTransform) {
    transform = parentTransform;

    rendered = true;
    update();

    return this;
}