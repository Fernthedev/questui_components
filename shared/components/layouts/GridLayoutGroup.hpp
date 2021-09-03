#pragma once

#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class GridLayoutGroup;
}

namespace QuestUI_Components {

    class GridLayoutGroup : public BaseContainer {
    public:
        explicit GridLayoutGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit GridLayoutGroup(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::GridLayoutGroup* gridLayoutGroup = nullptr;
    };
}