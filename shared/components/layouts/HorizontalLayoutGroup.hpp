#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class HorizontalLayoutGroup;
}

namespace QuestUI_Components {

    class HorizontalLayoutGroup : public Component, public Container {
    public:
        explicit HorizontalLayoutGroup(std::initializer_list<ComponentWrapper> children) : Container(children) {}

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        UnityEngine::Transform* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::HorizontalLayoutGroup* horizontalLayoutGroup = nullptr;
    };
}