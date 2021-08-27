#pragma once

#include "Component.hpp"
#include "RootContainer.hpp"

#include <string>
#include <utility>
#include <vector>

namespace QuestUI_Components {

    class HoverHint : public Component, public ComponentRenderer {
    public:
        explicit HoverHint(std::string_view hint, ComponentWrapper child) : text(hint), child(std::move(child)) {}

    protected:
        UnityEngine::Transform* render(UnityEngine::Transform *parentTransform) override;

    private:
        const std::string_view text;
        ComponentWrapper child;
    };
}