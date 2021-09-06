#pragma once

#include "UnityEngine/Vector2.hpp"

#include "BaseSetting.hpp"

#include <string>
#include <utility>
#include <vector>

namespace UnityEngine::UI {
    class Image;
}

namespace HMUI {
    class InputFieldView;
}

namespace TMPro {
    class TextMeshPro;
    class TextMeshProUGUI;
}

namespace QuestUI_Components {

    using MutableStringSettingsData = MutableSettingsData<std::string>;

    class StringSetting : public BaseSetting<std::string, StringSetting, MutableStringSettingsData> {
    public:
        struct InitStringSettingsData {
            UnityEngine::Vector2 anchoredPosition;
            UnityEngine::Vector3 keyboardPositionOffset;
        };

        explicit StringSetting(std::string_view text, std::string_view currentValue,
                               OnCallback callback = nullptr, std::optional<InitStringSettingsData> stringData = std::nullopt)
                               : BaseSetting(text, std::string(currentValue), std::move(callback)),
        stringInitData(stringData) {}

    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;
    private:
        // render time
        HMUI::InputFieldView* uiString = nullptr;

        // constructor time
        const std::optional<InitStringSettingsData> stringInitData;
    };



#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsStringSetting = ConfigUtilsSetting<std::string, StringSetting>;
#endif
}