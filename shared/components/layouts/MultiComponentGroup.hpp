#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class GridLayoutGroup;
}

namespace QuestUI_Components {

    class MultiComponentGroup : public BaseContainer {
    public:
        explicit MultiComponentGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;
    };
}