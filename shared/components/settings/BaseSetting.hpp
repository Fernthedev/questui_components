#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/concepts.hpp"
#include "shared/RootContainer.hpp"
#include "shared/components/HoverHint.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include <string>
#include <utility>
#include <vector>

#include "shared/state.hpp"


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

    template<typename T, typename Value>
    concept IsConfigType = requires(T const t) {
        {t.getValue()} -> QUC::IsQUCConvertible<Value>;

        typename T::OnCallback;
        IsQUCConvertible<typename T::OnCallback,std::function<void(T&, Value const&, UnityEngine::Transform *, RenderContext& ctx)>>;
    } && requires(T t, Value value) {
        {t.setValue(value)};
    };

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    template<typename ValueType, typename SettingType, typename ConfigValueType = ValueType>
    requires(IsConfigType<SettingType, ValueType>)
    class ConfigUtilsSetting : public SettingType {
    public:
        template<typename F, typename... TArgs>
        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable, TArgs&&... args) :
                ConfigUtilsSetting(configValue.GetValue(), configValue, std::forward<F>(callable), std::forward<TArgs>(args)...) {}

        template<typename... TArgs>
        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, std::nullptr_t nullCallback, TArgs&&... args) :
                ConfigUtilsSetting(configValue.GetValue(), configValue, buildCallback(configValue), std::forward<TArgs>(args)...) {}

        template<typename... TArgs>
        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) :
                ConfigUtilsSetting(configValue.GetValue(), configValue, buildCallback(configValue), std::forward<TArgs>(args)...) {}

//        template<typename F>
//        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable)
//        : ConfigUtilsSetting(configValue.GetValue(), configValue, callable) {}

        ValueType getValue() {
            return configValue.GetValue();
        }

        void setValue(const ValueType& val) {
            configValue.SetValue(val);
        }

        void resetChange() {}

        const Key key;


        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            SettingType::setValue(getValue());
            auto& hoverHint = data.getData<HMUI::HoverHint*>();
            Key parentKey = SettingType::key;
            auto res = SettingType::render(ctx, ctx.getChildData(parentKey));

            if (!configValue.GetHoverHint().empty() && !hoverHint) {
                hoverHint = QuestUI::BeatSaberUI::AddHoverHint(res->get_gameObject(), configValue.GetHoverHint());
            }

            return res;
        };

    protected:
//        template<typename F>
//        explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable) :
//                configValue(configValue), SettingType(configValue.GetName(), callable, currentValue) {}

        template<typename F, typename... TArgs>
        explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable, TArgs&&... args) :
                configValue(configValue), SettingType(configValue.GetName(), buildCallback<F>(configValue, std::forward<F>(callable)), currentValue, args...) {}

        template<typename... TArgs>
        explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) :
                configValue(configValue), SettingType(configValue.GetName(), buildCallback(configValue), currentValue, args...) {}

        template<typename F = typename SettingType::OnCallback const&>
        static typename SettingType::OnCallback buildCallback(ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callback) {
            return [callback, &configValue](auto& setting, ValueType const& val, UnityEngine::Transform* t, RenderContext& ctx) -> typename SettingType::OnCallback::result_type {
                configValue.SetValue(val);
                if (callback) {
                    return callback(setting, val, t, ctx);
                } else {
                    return {};
                }
            };
        }

        static typename SettingType::OnCallback buildCallback(ConfigUtils::ConfigValue<ConfigValueType>& configValue) {
            return [&configValue](auto& setting, ValueType const& val, UnityEngine::Transform* t, RenderContext& ctx) -> typename SettingType::OnCallback::result_type {
                configValue.SetValue(val);
                return {};
            };
        }

        // reference capture should be safe here
        ConfigUtils::ConfigValue<ConfigValueType>& configValue;
    };
#endif
}