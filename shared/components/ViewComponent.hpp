#pragma once

#include "RootContainer.hpp"
#include "Component.hpp"
#include <utility>
#include <vector>

namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    class ViewComponent : public ComponentRenderer {
    public:
        struct ViewComponentData {
            std::vector<ComponentWrapper> children;
        };

        ViewComponent(const UnityEngine::Transform *transform, ViewComponentData viewComponentData) : transform(transform), viewComponentData(std::move(viewComponentData)) {}

        // TODO: How to make this dynamically update existing children or add more only when needed?
        void render();


        void addMultipleToHierarchy(std::vector<ComponentWrapper> component);

        inline void addToHierarchy(ComponentWrapper component) {
            addMultipleToHierarchy({std::move(component)});
        }

    private:
        ViewComponentData viewComponentData;
        const UnityEngine::Transform* transform;
    };
}