#include "components/layouts/MultiComponentGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::MultiComponentGroup::render(UnityEngine::Transform *parentTransform) {
    transform = parentTransform;

    rendered = true;
    update();

    return this;
}

void QuestUI_Components::MultiComponentGroup::renderComponentInContainer(QuestUI_Components::ComponentWrapper &comp) {
    if (rendered) {
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
    }
}