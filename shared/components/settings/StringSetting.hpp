#pragma once

#include "UnityEngine/Vector2.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"
#include "components/HoverHint.hpp"

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

#include "UnityEngine/Transform.hpp"
#include "config-utils/shared/config-utils.hpp"

namespace QuestUI_Components {

    struct MutableStringSettingsData {
        // Mutable variables
        std::string stringText;

        bool enabled = true;
        bool interactable = true;

        friend class StringSetting;
    private:
        // To edit this, use stringValue() and setStringValue()
        std::string stringValue;
    };

    class StringSetting : public Component, public UpdateableComponent<MutableStringSettingsData> {
    public:
        using OnStringCallback = std::function<void(StringSetting*, std::string, UnityEngine::Transform*)>;

        struct InitStringSettingsData {
            UnityEngine::Vector2 anchoredPosition;
            UnityEngine::Vector3 keyboardPositionOffset;
        };

        explicit StringSetting(std::string_view text, std::string_view currentValue, OnStringCallback onString = nullptr, std::optional<InitStringSettingsData> stringData = std::nullopt) : stringInitData(stringData), onStringSet(std::move(onString)) {
            data.stringText = text;
            data.stringValue = currentValue;
        }

        // Allow constructing with lambda
        CONSTRUCT_AFTER_COMPONENT(StringSetting)


        // Override if needed. For example: config-utils
        virtual std::string stringValue() {
            return data.stringValue;
        }

        // This is always called on string set, whether your callback is nullptr or not.
        std::string setStringValue(std::string val) {
            bool different = stringValue() != val;
            updated |= different;
            if (different) {
                internalSetStringValue(val);
            }
            return val;
        }
    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        virtual void internalSetStringValue(std::string val);

        // Constructor time
    private:
        HMUI::InputFieldView* uiString = nullptr;
        TMPro::TextMeshProUGUI* uiStringText = nullptr;

        const std::optional<InitStringSettingsData> stringInitData;
        const OnStringCallback onStringSet;
    };



#ifndef AddConfigValueString
#define QC_AddConfigValueString
    class ConfigUtilsStringSetting : public StringSetting {
    public:
        explicit ConfigUtilsStringSetting(ConfigUtils::ConfigValue<std::string>& configValue) : configValue(std::ref(configValue)),
        StringSetting(configValue.GetName(), configValue.GetValue()) {
            if (!configValue.GetHoverHint().empty()) {
                hoverHint = std::make_shared<HoverHint>(configValue.GetHoverHint(), this);
            }
        }

    protected:
        // reference capture should be safe here
        std::shared_ptr<HoverHint> hoverHint;
        const std::reference_wrapper<ConfigUtils::ConfigValue<std::string>> configValue;


        std::string stringValue() override {
            return configValue.get().GetValue();
        }

        void internalSetStringValue(std::string val) override {
            configValue.get().SetValue(val);
        }

        Component *render(UnityEngine::Transform *parentTransform) override {
            StringSetting::render(parentTransform);

            if (hoverHint) {
                return hoverHint.get();
            } else {
                return this;
            }
        };

        void update() override {
            StringSetting::update();
        };
    };
#endif
}