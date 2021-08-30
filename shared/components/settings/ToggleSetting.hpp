#pragma once

#include "UnityEngine/Vector2.hpp"

#include "BaseSetting.hpp"

#include <string>
#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class Toggle;
    class Image;
}

namespace TMPro {
    class TextMeshPro;
    class TextMeshProUGUI;
}

namespace QuestUI_Components {

    using MutableToggleSettingsData = MutableSettingsData<bool>;

    class ToggleSetting : public BaseSetting<bool, ToggleSetting, MutableToggleSettingsData> {
    public:
        struct InitToggleSettingsData {
            UnityEngine::Vector2 anchoredPosition;
        };

        explicit ToggleSetting(std::string_view text, bool currentValue, OnCallback callback = nullptr,
                               std::optional<InitToggleSettingsData> toggleData = std::nullopt)
        : BaseSetting(std::string(text), currentValue, std::move(callback)),
                               toggleInitData(toggleData) {}

    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        // render time
        UnityEngine::UI::Toggle* uiToggle = nullptr;

        // Constructor time
        const std::optional<InitToggleSettingsData> toggleInitData;
    };



#if defined(AddConfigValue) || defined(AddConfigValueToggle)
    using ConfigUtilsToggleSetting = ConfigUtilsSetting<bool, ToggleSetting>;
#endif
}