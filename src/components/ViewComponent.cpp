#include "components/ViewComponent.hpp"

void QuestUI_Components::ViewComponent::render() {
    rendered = true;
    for (auto& comp : renderChildren)
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
}

void QuestUI_Components::ViewComponent::renderComponentInContainer(QuestUI_Components::ComponentWrapper &comp) {
    if (rendered) {
        renderComponent(comp, const_cast<UnityEngine::Transform*>(transform));
    }
}
