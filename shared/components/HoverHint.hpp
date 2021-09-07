#pragma once

#include "shared/Component.hpp"
#include "shared/RootContainer.hpp"

#include <string>
#include <utility>
#include <vector>

namespace QuestUI_Components {

    class HoverHint : public Component, public ComponentRenderer, public UpdateableComponentBase {
    public:
        explicit HoverHint(std::string_view hint, ComponentWrapper child) : text(hint), child(std::move(child)) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        void update() override;

    private:
        const std::string text;
        ComponentWrapper child;
    };
}