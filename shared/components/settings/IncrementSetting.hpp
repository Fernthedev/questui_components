#pragma once

#include "UnityEngine/Vector2.hpp"

#include "BaseSetting.hpp"

#include <string>
#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class Image;
}

namespace QuestUI {
    class IncrementSetting;
}

namespace TMPro {
    class TextMeshPro;
    class TextMeshProUGUI;
}

namespace QuestUI_Components {

    using MutableIncrementSettingsData = MutableSettingsData<float>;

    class IncrementSetting : public BaseSetting<float, IncrementSetting, MutableIncrementSettingsData> {
    public:
        struct InitIncrementSettingsData {
            std::optional<float> max = std::nullopt;
            std::optional<float> min = std::nullopt;
            UnityEngine::Vector2 anchoredPosition;
        };

        explicit IncrementSetting(std::string_view text, float currentValue, int decimals, float increment, OnCallback callback = nullptr,
                               std::optional<InitIncrementSettingsData> incrementData = std::nullopt)
        : BaseSetting(std::string(text), currentValue, std::move(callback)),
                               incrementInitData(incrementData),
                               decimals(decimals),
                               increment(increment) {}

    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        // render time
        QuestUI::IncrementSetting* uiIncrement = nullptr;

        // Constructor time
        int decimals;
        float increment;
        const std::optional<InitIncrementSettingsData> incrementInitData;
    };



#if defined(AddConfigValue) || defined(AddConfigValueIncrement)
    using ConfigUtilsIncrementSetting = ConfigUtilsSetting<float, IncrementSetting>;
#endif
}