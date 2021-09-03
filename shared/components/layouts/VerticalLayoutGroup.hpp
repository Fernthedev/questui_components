#pragma once


#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class VerticalLayoutGroup;
}

namespace QuestUI_Components {

    class VerticalLayoutGroup : public BaseContainer {
    public:
        explicit VerticalLayoutGroup(std::initializer_list<ComponentWrapper> children) : BaseContainer(children) {}
        explicit VerticalLayoutGroup(std::vector<ComponentWrapper> children) : BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        UnityEngine::UI::VerticalLayoutGroup* verticalLayoutGroup = nullptr;
    };
}