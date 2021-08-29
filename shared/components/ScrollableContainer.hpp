#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {

    class ScrollableContainer : public Component, public Container {
    public:
        explicit ScrollableContainer(std::initializer_list<ComponentWrapper> children) : Container(children) {}

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        Component* render(UnityEngine::Transform *parentTransform) override;
    };
}