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
        constexpr explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable, TArgs&&... args) :
                ConfigUtilsSetting(configValue.GetValue(), configValue, std::forward<F>(callable), std::forward<TArgs>(args)...) {}

        template<typename... TArgs>
        constexpr explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, std::nullptr_t nullCallback, TArgs&&... args) :
                ConfigUtilsSetting(configValue.GetValue(), configValue, buildCallback(configValue), std::forward<TArgs>(args)...) {}

        template<typename... TArgs>
        constexpr explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) :
                ConfigUtilsSetting(configValue.GetValue(), configValue, buildCallback(configValue), std::forward<TArgs>(args)...) {}

//        template<typename F>
//        explicit ConfigUtilsSetting(ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable)
//        : ConfigUtilsSetting(configValue.GetValue(), configValue, callable) {}

        constexpr ValueType getValue() const noexcept {
            return configValue.GetValue();
        }

        constexpr void setValue(const ValueType& val) noexcept {
            configValue.SetValue(val);
        }

        void resetChange() {}


        constexpr UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            // Update the component to read the config value
            SettingType::setValue(getValue());
            auto& hoverHint = data.getData<HMUI::HoverHint*>();
            auto res = SettingType::render(ctx, ctx.getChildData(SettingType::key));

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
        constexpr explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callable, TArgs&&... args) :
                configValue(configValue), SettingType(configValue.GetName(), buildCallback<F>(configValue, std::forward<F>(callable)), currentValue, args...) {}

        template<typename... TArgs>
        constexpr explicit ConfigUtilsSetting(ValueType currentValue, ConfigUtils::ConfigValue<ConfigValueType>& configValue, TArgs&&... args) :
                configValue(configValue), SettingType(configValue.GetName(), buildCallback(configValue), currentValue, args...) {}

        template<typename F = typename SettingType::OnCallback const&>
        constexpr static typename SettingType::OnCallback buildCallback(ConfigUtils::ConfigValue<ConfigValueType>& configValue, F&& callback) {
            return [callback, &configValue](auto& setting, ValueType const& val, UnityEngine::Transform* t, RenderContext& ctx) -> typename SettingType::OnCallback::result_type {
                configValue.SetValue(val);

                if constexpr (std::is_convertible_v<F, bool>) {
                    if (!callback) return {};
                }

                return callback(setting, val, t, ctx);
            };
        }

        constexpr static typename SettingType::OnCallback buildCallback(ConfigUtils::ConfigValue<ConfigValueType>& configValue) {
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