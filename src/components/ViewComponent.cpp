#include "components/ViewComponent.hpp"

void QuestUI_Components::ViewComponent::render() {
    std::vector<ComponentWrapper> childrenCopy(viewComponentData.children);
    viewComponentData.children.clear();
    addMultipleToHierarchy(childrenCopy);
}

void QuestUI_Components::ViewComponent::addMultipleToHierarchy(std::vector<ComponentWrapper> components) {
    for (auto& component : components) {
        renderComponent(component, const_cast<UnityEngine::Transform*>(transform));
        viewComponentData.children.emplace_back(component);
    }
}
