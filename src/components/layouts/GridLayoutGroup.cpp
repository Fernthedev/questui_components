#include "components/layouts/GridLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;

UnityEngine::Transform* QuestUI_Components::GridLayoutGroup::render(UnityEngine::Transform *parentTransform) {
    gridLayoutGroup = BeatSaberUI::CreateGridLayoutGroup(parentTransform);

    transform = gridLayoutGroup->get_transform();

    rendered = true;
    for (auto& comp : renderChildren)
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));

    return transform;
}

void QuestUI_Components::GridLayoutGroup::renderComponentInContainer(QuestUI_Components::ComponentWrapper &comp) {
    if (rendered) {
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
    }
}