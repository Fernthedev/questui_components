#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {

    class ScrollableContainer : public Component, public Container {
    public:
        explicit ScrollableContainer(std::vector<ComponentWrapper> children) {
            renderChildren = std::move(children);
        }

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

        UnityEngine::Transform* render(UnityEngine::Transform *parentTransform) override;
    };
}