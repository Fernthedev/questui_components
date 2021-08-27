#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <utility>
#include <vector>

namespace QuestUI_Components {

    class ScrollableContainer : public Component, public ComponentRenderer {
    public:
        explicit ScrollableContainer(std::vector<ComponentWrapper> children) : children(std::move(children)) {}


        void addMultipleToHierarchy(std::vector<ComponentWrapper> component);

        inline void addToHierarchy(ComponentWrapper component) {
            addMultipleToHierarchy({std::move(component)});
        }
    protected:
        UnityEngine::Transform* render(UnityEngine::Transform *parentTransform) override;

    private:
        std::vector<ComponentWrapper> children;
    };
}