#include "components/ScrollableContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "main.hpp"

using namespace QuestUI;

UnityEngine::Transform* QuestUI_Components::ScrollableContainer::render(UnityEngine::Transform *parentTransform) {
    UnityEngine::GameObject* scrollableContainer = BeatSaberUI::CreateScrollableSettingsContainer(parentTransform);

    transform = scrollableContainer->get_transform();

    std::vector<ComponentWrapper> childrenCopy(children);
    children.clear();
    addMultipleToHierarchy(childrenCopy);

    return transform;
}

void QuestUI_Components::ScrollableContainer::addMultipleToHierarchy(std::vector<ComponentWrapper> components) {
    for (auto& component : components) {
        getLogger().debug("Transform name: %s", to_utf8(csstrtostr(transform->get_name())).c_str());
        renderComponent(component, const_cast<UnityEngine::Transform*>(transform));
        children.emplace_back(component);
    }
}
