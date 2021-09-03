#pragma once

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"

#include "shared/Component.hpp"

#include <string>
#include <vector>


namespace UnityEngine {
    class Sprite;
    namespace UI {
        class Image;
    }
}


namespace QuestUI_Components {
    struct MutableImageData {
        bool enabled = true;

        // cannot be null
        UnityEngine::Sprite* sprite;
    };


    class Image : public Component, public UpdateableComponent<MutableImageData> {
    public:
        struct InitialImageData {
            UnityEngine::Vector2 sizeDelta;
            UnityEngine::Vector2 anchoredPosition = {0.0f, 0.0f};
        };
        explicit Image(UnityEngine::Sprite* sprite, InitialImageData imageData) : initialImageData(imageData) {
            data.sprite = sprite;

            if (!data.sprite)
                throw std::runtime_error("Sprite is null! Cannot happen");
        }

        CONSTRUCT_AFTER_COMPONENT(Image)

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;
        void update() override;

        const InitialImageData initialImageData;

        // render time
        UnityEngine::UI::Image* uiImage = nullptr;
    };
}