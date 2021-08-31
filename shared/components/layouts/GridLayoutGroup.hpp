#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class GridLayoutGroup;
}

namespace QuestUI_Components {

    class GridLayoutGroup : public BaseContainer {
    public:
        explicit GridLayoutGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::GridLayoutGroup* gridLayoutGroup = nullptr;
    };
}