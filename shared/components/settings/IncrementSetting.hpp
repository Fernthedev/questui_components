#pragma once

#include "BaseSetting.hpp"

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include <string>

namespace QUC {
    struct IncrementSetting {
    private:
        struct RenderIncrementSetting {
            QuestUI::IncrementSetting* setting;
            TMPro::TextMeshProUGUI* textSetting;
        };
    public:

        using OnCallback = std::function<void(IncrementSetting&, float, UnityEngine::Transform*, RenderContext& ctx)>;
        HeldData<std::string> text;
        OnCallback callback;
        HeldData<bool> enabled;
        HeldData<bool> interactable;
        HeldData<float> value;
        HeldData<int> decimals;
        HeldData<float> increment;
        HeldData<std::optional<float>> min;
        HeldData<std::optional<float>> max;
        const UnityEngine::Vector2 anchoredPosition;
        const Key key;

        template<class F>
        IncrementSetting(std::string_view txt, F&& callable, float currentValue = 0.0f, int decimals_ = 1, float increment = 1.0f, std::optional<float> min_ = std::nullopt, std::optional<float> max_ = std::nullopt,  bool enabled_ = true, bool interact = true, UnityEngine::Vector2 anch = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), increment(increment), decimals(decimals_), min(min_), max(max_), anchoredPosition(anch) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto &settingData = data.getData<RenderIncrementSetting>();
            auto &setting = settingData.setting;

            auto parent = &ctx.parentTransform;
            if (!setting) {
                auto cbk = std::function<void(float)>(
                        [callback = this->callback, parent, &ctx, this](float val) mutable {
                            value = val;
                            value.clear();

                            if (callback)
                                callback(*this, val, parent, ctx);
                        });

                setting = QuestUI::BeatSaberUI::CreateIncrementSetting(
                        parent,
                        *text,
                        *decimals,
                        *increment,
                        *value,
                        static_cast<bool>(*min),
                        static_cast<bool>(*max),
                        min.getData().value_or(0.0f),
                        max.getData().value_or(0.0f),
                        anchoredPosition,
                        cbk);

                assign<true>(settingData);
            } else {
                assign<false>(settingData);
            }


            return setting->get_transform();
        }

        void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& inputFieldView = data.getData<RenderIncrementSetting>();

            assign<false>(inputFieldView);
        }

    protected:
        template<bool created = false>
        void assign(RenderIncrementSetting& renderIncrementSetting) {
            auto setting = renderIncrementSetting.setting;
            auto& textSetting = renderIncrementSetting.textSetting;
            CRASH_UNLESS(setting);

            if (enabled) {
                setting->set_enabled(*enabled);
                enabled.clear();
            }

            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            // TODO: Interactable

            if constexpr(!created) {

                if (text) {
                    if (!textSetting)
                        textSetting = setting->GetComponentInChildren<TMPro::TextMeshProUGUI *>();

                    CRASH_UNLESS(textSetting);
                    textSetting->set_text(il2cpp_utils::newcsstr(*text));
                    text.clear();
                }
                if (decimals) {
                    setting->Decimals = *decimals;
                    decimals.clear();
                }
                if (increment) {
                    setting->Increment = *increment;
                    increment.clear();
                }
                if (max) {
                    setting->MaxValue = max.getData().value_or(0);
                    setting->HasMax = static_cast<bool>(max.getData());
                    max.clear();
                }
                if (min) {
                    setting->MinValue = min.getData().value_or(0);
                    setting->HasMin = static_cast<bool>(min.getData());
                    min.clear();
                }

                if (value) {
                    setting->CurrentValue = *value;
                    value.clear();
                }
            }
        }

    };

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsIncrementSetting = ConfigUtilsSetting<float, IncrementSetting>;
#endif
    static_assert(renderable<IncrementSetting>);
}
