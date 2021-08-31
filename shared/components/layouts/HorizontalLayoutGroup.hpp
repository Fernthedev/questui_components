#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class HorizontalLayoutGroup;
}

namespace QuestUI_Components {

    class HorizontalLayoutGroup : public BaseContainer {
    public:
        explicit HorizontalLayoutGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::HorizontalLayoutGroup* horizontalLayoutGroup = nullptr;
    };
}