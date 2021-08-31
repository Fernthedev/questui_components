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
        ViewComponent(const UnityEngine::Transform *transform, std::initializer_list<ComponentPtrWrapper> components) :
        transform(transform),
        Container(components) {}

        void render();

    protected:
        void renderComponentInContainer(ComponentWrapper &comp) override;

    private:
        bool rendered = false;
        const UnityEngine::Transform* transform;
    };
}