#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/RootContainer.hpp"
#include "shared/components/HoverHint.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include <string>
#include <utility>
#include <vector>

#include "shared/state.hpp"
#include "shared/unity/WeakPtrGO.hpp"

#include "UnityEngine/Transform.hpp"
#include "config-utils/shared/config-utils.hpp"

namespace UnityEngine::UI {
    class Image;
}

namespace TMPro {
    class TextMeshProUGUI;
}

namespace QUC {
//    template<typename ValueType, typename SettingType>
//    class BaseSetting {
//    public:
//        // Mutable variables
//        HeldData<std::string> text;
//        HeldData<bool> enabled = true;
//        HeldData<bool> interactable = true;
//
//        using OnCallback = std::function<void(SettingType*, ValueType, UnityEngine::Transform*)>;
//        using CallbackWrapper = std::function<void(ValueType)>;
//
//        constexpr BaseSetting(std::string_view text, OnCallback callback = nullptr, bool enabled = true, bool interactable = true) : text(text),
//                enabled(enabled),
//                interactable(interactable)
//                {}
//
//    void resetChange() {
//
//    }
//
//
//    protected:
//        CallbackWrapper constructWrapperCallback(UnityEngine::Transform *parentTransform) {
//            if (callback) {
//                return [this, parentTransform](ValueType val) {
//                    this->setValue(val);
//                    this->resetChange();
//                    // better way of doing this than reinterpret cast to child?
//                    callback(reinterpret_cast<SettingType*>(this), val, parentTransform);
//                };
//            } else {
//                return [this](ValueType val) {
//                    this->setValue(val);
//                    this->resetChange();
//                };
//            }
//        }
//
//        void update() {
//            if (uiText && text) {
//                uiText->set_text(il2cpp_utils::newcsstr(*text));
//                text.clear();
//            }
//        }
//
//
//        // render time
//        WeakPtrGO<TMPro::TextMeshProUGUI> uiText;
//
//        // constructor time
//        const OnCallback callback;
//    };
//
//    template<typename ValueType, typename SettingType>
//    class SimpleBaseSetting : public BaseSetting<ValueType, SettingType> {
//        HeldData<ValueType> value;
//
//
//        // Override if needed. For example: config-utils
//        ValueType getValue() {
//            return this->value;
//        }
//
//        // This is always called on set, whether your callback is nullptr or not.
//        ValueType setValue(ValueType val) {
//            return value = val;
//        }
//
//        void resetChange() {
//            value.clear();
//        }
//    };


#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    template<typename ValueType, typename SettingType, typename ConfigValueType = ValueType>
    class ConfigUtilsSetting : public SettingType {
    public:
        template<typename... TArgs>
        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) : ConfigUtilsSetting(configValue.GetValue(), configValue, args...) {}

        ValueType getValue() override {
            return configValue.GetValue();
        }

        void setValue(const ValueType& val) override {
            configValue.SetValue(val);
        }

        void resetChange() {}

        void update() override {
            SettingType::update();
        }

    protected:
        template<typename... TArgs>
        explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) :
                configValue(configValue), SettingType(configValue.GetName(), currentValue, args...) {}


        // reference capture should be safe here
        ConfigUtils::ConfigValue<ConfigValueType>& configValue;
        const Key key;


        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& hoverHint = data.getData<HMUI::HoverHint*>();
            Key parentKey = SettingType::key;
            auto res = SettingType::render(ctx, ctx.getChildData(parentKey));

            if (!configValue.GetHoverHint().empty() && !hoverHint) {
                hoverHint = QuestUI::BeatSaberUI::AddHoverHint(res->get_gameObject(), configValue.GetHoverHint());
            }

            return res;
        };
    };
#endif
}