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
        std::string_view text;
        bool enabled = true;
        bool interactable = true;
        std::optional<UnityEngine::UI::Image*> image;
    };

    class Button : public Component, public IRenderOnce, public UpdateableComponent<MutableButtonData> {
    public:
        using onClickCallback = std::function<void(UnityEngine::Transform*)>;

        struct InitButtonData {
            std::string_view buttonTemplate;
            UnityEngine::Vector2 anchoredPosition;
            UnityEngine::Vector2 sizeDelta;
        };

        explicit Button(std::string_view text, onClickCallback onClick = nullptr, std::optional<InitButtonData> buttonData = std::nullopt) : buttonData(buttonData), onClick(std::move(onClick)) {
            data.text = text;
        }

        // Allow constructing with lambda
        CONSTRUCT_AFTER_COMPONENT(Button)
    protected:
        void update() override;
        UnityEngine::Transform* render(UnityEngine::Transform *parentTransform) override;

        // Constructor time
    private:
        UnityEngine::UI::Button* uiButton = nullptr;
        TMPro::TextMeshProUGUI* uiButtonText = nullptr;

        std::optional<InitButtonData> buttonData;
        onClickCallback onClick;
    };
}