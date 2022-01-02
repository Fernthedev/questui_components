#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include <string>

namespace QUC {
    struct IncrementSetting {
        using OnCallback = std::function<void(IncrementSetting*, float, UnityEngine::Transform*)>;
        std::string text;
        OnCallback callback;
        bool enabled;
        bool interactable;
        float value;
        int decimals;
        float increment;
        std::optional<float> min;
        std::optional<float> max;
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
                        text,
                        decimals,
                        increment,
                        value,
                        static_cast<bool>(min),
                        static_cast<bool>(max),
                        min.value_or(0.0f),
                        max.value_or(0.0f),
                        anchoredPosition,
                        std::function<void(float)>([this, parent](float val) {
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

            if constexpr(!created) {
                auto txt = setting->GetComponentInChildren<TMPro::TextMeshProUGUI *>();;

                CRASH_UNLESS(txt);
                txt->set_text(il2cpp_utils::newcsstr(text));
                setting->set_enabled(enabled);
                setting->Decimals = decimals;
                setting->Increment = increment;

                setting->MaxValue = max.value_or(0);
                setting->MinValue = min.value_or(0);

                setting->HasMax = static_cast<bool>(max);
                setting->HasMin = static_cast<bool>(min);

                setting->CurrentValue = value;
            }
        }

    private:
        WeakPtrGO<QuestUI::IncrementSetting> setting;
    };
    static_assert(renderable<IncrementSetting>);
}
