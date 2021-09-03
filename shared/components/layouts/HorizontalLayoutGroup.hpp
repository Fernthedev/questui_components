#pragma once

#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class HorizontalLayoutGroup;
}

namespace QuestUI_Components {

    class HorizontalLayoutGroup : public BaseContainer {
    public:
        explicit HorizontalLayoutGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit HorizontalLayoutGroup(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::HorizontalLayoutGroup* horizontalLayoutGroup = nullptr;
    };
}