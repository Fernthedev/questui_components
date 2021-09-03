#pragma once

#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {

    class ScrollableContainer : public BaseContainer {
    public:
        explicit ScrollableContainer(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit ScrollableContainer(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;
    };
}