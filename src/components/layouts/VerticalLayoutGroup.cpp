#include "components/layouts/VerticalLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::VerticalLayoutGroup::render(UnityEngine::Transform *parentTransform) {
    verticalLayoutGroup = BeatSaberUI::CreateVerticalLayoutGroup(parentTransform);

    transform = verticalLayoutGroup->get_transform();

    rendered = true;
    for (auto& comp : renderChildren)
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));

    return this;
}

void QuestUI_Components::VerticalLayoutGroup::renderComponentInContainer(QuestUI_Components::ComponentWrapper &comp) {
    if (rendered) {
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
    }
}