#pragma once

#include "shared/components/Text.hpp"
#include "custom-types/shared/coroutine.hpp"

#include <utility>
#include <vector>
#include <string>


namespace UnityEngine {
    class Transform;
}

namespace QuestUI_Components {
    class RainbowText : public Text {
    public:
        explicit RainbowText(std::string_view prefix) : Text(prefix) {}

    float speed = 1.0f;

    protected:
        Component* render(UnityEngine::Transform *parentTransform) override;

        custom_types::Helpers::Coroutine rainbowCoroutine();

        int progress;

    };
}