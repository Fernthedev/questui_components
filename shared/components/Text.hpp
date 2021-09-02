#pragma once

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"

#include <string>
#include <vector>

namespace TMPro {
    class TextMeshPro;
    class TextMeshProUGUI;
}

namespace QuestUI_Components {

    struct MutableTextData {
        std::string text;
        bool enabled = true;
        std::optional<UnityEngine::Color> color = std::nullopt;
        std::optional<float> fontSize;
    };

    class Text : public Component, public UpdateableComponent<MutableTextData> {
    public:
        struct InitialTextData {
            bool italic = true;
            UnityEngine::Vector2 anchoredPosition = {0.0f, 0.0f};
            UnityEngine::Vector2 sizeDelta = {60.0f, 10.0f};
        };
        explicit Text(std::string_view text, std::optional<InitialTextData> textData = std::nullopt) : initialTextData(textData) {
            data.text = text;
        }

        CONSTRUCT_AFTER_COMPONENT(Text)

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;
        void update() override;

        const std::optional<InitialTextData> initialTextData;

        // render time
        TMPro::TextMeshProUGUI* textUI = nullptr;
    };
}