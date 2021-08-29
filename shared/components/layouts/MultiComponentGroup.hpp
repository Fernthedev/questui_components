#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class GridLayoutGroup;
}

namespace QuestUI_Components {

    class MultiComponentGroup : public Component, public Container {
    public:
        explicit MultiComponentGroup(std::initializer_list<ComponentWrapper> children) : Container(children) {}

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        Component* render(UnityEngine::Transform *parentTransform) override;
    };
}