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

    /**
     * @brief Modifies the background of the child
     */
    class Backgroundable : public Component, public ComponentRenderer {
    public:
        explicit Backgroundable(std::string_view backgroundType, ComponentWrapper child, bool replaceExistingBackground = true) : replaceExistingBackground(replaceExistingBackground), backgroundType(backgroundType), child(std::move(child)) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        // render time
        QuestUI::Backgroundable* background = nullptr;

    private:
        // constructor time
        const bool replaceExistingBackground;
        std::string backgroundType;
        ComponentWrapper child;
    };

    /**
     * @brief Creates a new object and modifies the background
     */
    class BackgroundableContainer : public BaseContainer {
    public:
        explicit BackgroundableContainer(std::string_view backgroundType, std::initializer_list<ComponentWrapper> children) :  backgroundType(backgroundType), BaseContainer(children) {}
        explicit BackgroundableContainer(std::string_view backgroundType, std::vector<ComponentWrapper> const& children) :  backgroundType(backgroundType), BaseContainer(children) {}

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        // render time
        QuestUI::Backgroundable* background = nullptr;

    private:
        // construct type
        std::string backgroundType;
    };
}