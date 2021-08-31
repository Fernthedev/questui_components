#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {

    class ScrollableContainer : public BaseContainer {
    public:
        explicit ScrollableContainer(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;
    };
}