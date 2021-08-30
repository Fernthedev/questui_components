#pragma once

#include "UnityEngine/Vector2.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"

#include <string>
#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class Button;
    class Image;
}

namespace TMPro {
    class TextMeshPro;
    class TextMeshProUGUI;
}

namespace QuestUI_Components {

    struct MutableButtonData {
        // Mutable variables
        std::string text;
        bool enabled = true;
        bool interactable = true;
        std::optional<UnityEngine::UI::Image*> image;
    };

    class Button : public Component, public UpdateableComponent<MutableButtonData> {
    public:
        using onClickCallback = std::function<void(Button* button, UnityEngine::Transform*)>;

        struct InitButtonData {
            std::string buttonTemplate;
            UnityEngine::Vector2 anchoredPosition;
            UnityEngine::Vector2 sizeDelta;
        };

        explicit Button(std::string_view text, onClickCallback onClick = nullptr, std::optional<InitButtonData> buttonData = std::nullopt) : buttonData(std::move(buttonData)), onClick(std::move(onClick)) {
            data.text = text;
        }

        // Allow constructing with lambda
        CONSTRUCT_AFTER_COMPONENT(Button)
    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        // Constructor time
        UnityEngine::UI::Button* uiButton = nullptr;
        TMPro::TextMeshProUGUI* uiButtonText = nullptr;

        const std::optional<InitButtonData> buttonData;
        const onClickCallback onClick;
    };
}