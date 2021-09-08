#include "shared/components/layouts/ModifierContainer.hpp"

using namespace QuestUI_Components;

Component *QuestUI_Components::ModifierContainer::render(UnityEngine::Transform *parentTransform) {
    verticalLayoutGroup = CreateModifierContainer(parentTransform);

    transform = verticalLayoutGroup->get_transform();

    rendered = true;
    update();

    return this;
}
