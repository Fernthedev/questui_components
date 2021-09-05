#pragma once

#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {

    class MultiComponentGroup : public BaseContainer {
    public:
        explicit MultiComponentGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit MultiComponentGroup(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;
    };
}