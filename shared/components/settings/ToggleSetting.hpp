#pragma once

#include "UnityEngine/Vector2.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"
#include "components/HoverHint.hpp"

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

#include "UnityEngine/Transform.hpp"
#include "config-utils/shared/config-utils.hpp"

namespace QuestUI_Components {

    struct MutableToggleSettingsData {
        // Mutable variables
        std::string toggleText;

        bool enabled = true;
        bool interactable = true;

        friend class ToggleSetting;
    private:
        // To edit this, use toggleValue() and setToggleValue()
        bool toggleValue = false;
    };

    class ToggleSetting : public Component, public UpdateableComponent<MutableToggleSettingsData> {
    public:
        using onToggleCallback = std::function<void(ToggleSetting*, bool, UnityEngine::Transform*)>;

        struct InitToggleSettingsData {
            UnityEngine::Vector2 anchoredPosition;
        };

        explicit ToggleSetting(std::string_view text, bool currentValue, onToggleCallback onToggle = nullptr, std::optional<InitToggleSettingsData> toggleData = std::nullopt) : toggleInitData(toggleData), onToggle(std::move(onToggle)) {
            data.toggleText = text;
            data.toggleValue = currentValue;
        }

        // Allow constructing with lambda
        CONSTRUCT_AFTER_COMPONENT(ToggleSetting)


        // Override if needed. For example: config-utils
        virtual bool toggleValue() {
            return data.toggleValue;
        }

        // This is always called on toggle set, whether your callback is nullptr or not.
        bool setToggleValue(bool val) {
            bool different = toggleValue() != val;
            updated |= different;
            if (different) {
                internalSetToggleValue(val);
            }
            return val;
        }
    protected:
        void update() override;
        Component* render(UnityEngine::Transform *parentTransform) override;

        virtual void internalSetToggleValue(bool val);

        // Constructor time
    private:
        UnityEngine::UI::Toggle* uiToggle = nullptr;
        TMPro::TextMeshProUGUI* uiToggleText = nullptr;

        const std::optional<InitToggleSettingsData> toggleInitData;
        const onToggleCallback onToggle;
    };



#ifdef AddConfigValueToggle
#define QC_AddConfigValueToggle
    class ConfigUtilsToggleSetting : public ToggleSetting {
    public:
        explicit ConfigUtilsToggleSetting(ConfigUtils::ConfigValue<bool>& configValue) : configValue(std::ref(configValue)),
        ToggleSetting(configValue.GetName(), configValue.GetValue()) {
            if (!configValue.GetHoverHint().empty()) {
                hoverHint = std::make_shared<HoverHint>(configValue.GetHoverHint(), this);
            }
        }

    protected:
        // reference capture should be safe here
        std::shared_ptr<HoverHint> hoverHint;
        const std::reference_wrapper<ConfigUtils::ConfigValue<bool>> configValue;


        bool toggleValue() override {
            return configValue.get().GetValue();
        }

        void internalSetToggleValue(bool val) override {
            configValue.get().SetValue(val);
        }

        Component *render(UnityEngine::Transform *parentTransform) override {
            ToggleSetting::render(parentTransform);

            if (hoverHint) {
                return hoverHint.get();
            } else {
                return this;
            }
        };

        void update() override {
            ToggleSetting::update();
        };
    };
#endif
}