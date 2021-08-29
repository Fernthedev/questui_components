#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class VerticalLayoutGroup;
}

namespace QuestUI_Components {

    class VerticalLayoutGroup : public Component, public Container {
    public:
        explicit VerticalLayoutGroup(std::initializer_list<ComponentWrapper> children) : Container(children) {}

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::VerticalLayoutGroup* verticalLayoutGroup = nullptr;
    };
}