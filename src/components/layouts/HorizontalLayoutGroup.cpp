#include "components/layouts/HorizontalLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace QuestUI_Components;

Component* QuestUI_Components::HorizontalLayoutGroup::render(UnityEngine::Transform *parentTransform) {
    horizontalLayoutGroup = BeatSaberUI::CreateHorizontalLayoutGroup(parentTransform);

    transform = horizontalLayoutGroup->get_transform();

    rendered = true;
    for (auto& comp : renderChildren)
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));

    return this;
}

void QuestUI_Components::HorizontalLayoutGroup::renderComponentInContainer(QuestUI_Components::ComponentWrapper &comp) {
    if (rendered) {
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
    }
}