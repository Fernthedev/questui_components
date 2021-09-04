#pragma once

#include "shared/Component.hpp"
#include "shared/RootContainer.hpp"

#include <string>
#include <utility>
#include <vector>

namespace QuestUI {
    class Backgroundable;
}

namespace QuestUI_Components {

    class Backgroundable : public Component, public ComponentRenderer {
    public:
        explicit Backgroundable(std::string_view backgroundType, ComponentWrapper child, bool replaceExistingBackground = true) : replaceExistingBackground(replaceExistingBackground), backgroundType(backgroundType), child(std::move(child)) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        QuestUI::Backgroundable* background;

    private:
        const bool replaceExistingBackground;
        std::string backgroundType;
        ComponentWrapper child;
    };
}