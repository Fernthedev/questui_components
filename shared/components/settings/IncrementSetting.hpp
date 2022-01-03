#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include <string>

namespace QUC {
    struct IncrementSetting {
        using OnCallback = std::function<void(IncrementSetting*, float, UnityEngine::Transform*)>;
        HeldData<std::string> text;
        OnCallback callback;
        HeldData<bool> enabled;
        HeldData<bool> interactable;
        HeldData<float> value;
        HeldData<int> decimals;
        HeldData<float> increment;
        HeldData<std::optional<float>> min;
        HeldData<std::optional<float>> max;
        UnityEngine::Vector2 anchoredPosition;

        template<class F>
        IncrementSetting(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, float currentValue = 0.0f, int decimals_ = 1, float increment = 1.0f, std::optional<float> min_ = std::nullopt, std::optional<float> max_ = std::nullopt, UnityEngine::Vector2 anch = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), decimals(decimals_), min(min_), max(max_), anchoredPosition(anch) {}

        auto render(RenderContext& ctx) {
            // TODO: Cache this properly
            auto parent = &ctx.parentTransform;
            if (!setting) {
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
                        std::function<void(float)>([this, parent](float val) {
                            value = val;
                            value.clear();
                            callback(this, val, parent);
                        }));

                assign<true>();
            } else {
                assign<false>();
            }



            return setting->get_transform();
        }

        template<bool created = false>
        void assign() {
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
                    max.clear();
                }
                if (min) {
                    setting->MinValue = min.getData().value_or(0);
                    min.clear();
                }

                if (max) {
                    setting->HasMax = static_cast<bool>(max.getData());
                }

                if (min) {
                    setting->HasMin = static_cast<bool>(min.getData());
                }

                if (value) {
                    setting->CurrentValue = *value;
                }
            }
        }

    private:
        WeakPtrGO<QuestUI::IncrementSetting> setting;
        WeakPtrGO<TMPro::TextMeshProUGUI> textSetting;
    };
    static_assert(renderable<IncrementSetting>);
}
