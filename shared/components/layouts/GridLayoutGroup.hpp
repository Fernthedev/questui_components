#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class GridLayoutGroup;
}

namespace QuestUI_Components {

    class GridLayoutGroup : public Component, public Container {
    public:
        explicit GridLayoutGroup(std::initializer_list<ComponentWrapper> children) : Container(children) {}

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        UnityEngine::Transform* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::GridLayoutGroup* gridLayoutGroup = nullptr;
    };
}