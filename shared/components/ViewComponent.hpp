#pragma once

#include "RootContainer.hpp"
#include "Component.hpp"
#include <utility>
#include <vector>

namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    class ViewComponent : public Container {
    public:
        struct ViewComponentData {
            std::vector<ComponentWrapper> children;
        };

        ViewComponent(const UnityEngine::Transform *transform, ViewComponentData viewComponentData) : transform(transform), viewComponentData(std::move(viewComponentData)) {
            renderChildren = this->viewComponentData.children;
        }

        void render();

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

    private:
        bool rendered = false;
        const ViewComponentData viewComponentData;
        const UnityEngine::Transform* transform;
    };
}