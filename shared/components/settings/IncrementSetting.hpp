#pragma once

#include "BaseSetting.hpp"

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include <string>

namespace QUC {
    template<bool copySelfOnCallback = true>
    struct BasicIncrementSetting {
    private:
        struct RenderIncrementSetting {
            QuestUI::IncrementSetting* setting;
            TMPro::TextMeshProUGUI* textSetting;
        };
    public:

        using OnCallback = std::function<void(BasicIncrementSetting&, float, UnityEngine::Transform*, RenderContext& ctx)>;
        RenderHeldData<std::string> text;
        OnCallback callback;
        RenderHeldData<bool> enabled;
        RenderHeldData<bool> interactable;
        RenderHeldData<float> value;
        RenderHeldData<int> decimals;
        RenderHeldData<float> increment;
        RenderHeldData<std::optional<float>> min;
        RenderHeldData<std::optional<float>> max;
        const UnityEngine::Vector2 anchoredPosition;
        const Key key;

        template<class F>
        constexpr BasicIncrementSetting(std::string_view txt, F&& callable, float currentValue = 0.0f, int decimals_ = 1, float increment = 1.0f, std::optional<float> min_ = std::nullopt, std::optional<float> max_ = std::nullopt,  bool enabled_ = true, bool interact = true, UnityEngine::Vector2 anch = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), increment(increment), decimals(decimals_), min(min_), max(max_), anchoredPosition(anch) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto &settingData = data.getData<RenderIncrementSetting>();
            auto &setting = settingData.setting;

            auto parent = &ctx.parentTransform;
            if (!setting) {
                std::function<void(float)> onValueChange;

                if constexpr (copySelfOnCallback) {
                    auto c = *this;
                    onValueChange = std::function<void(float)>(
                            [callback = this->callback, parent, &ctx, c](float val) mutable {
                                c.value = val;
                                c.value.markCleanForRender(ctx);

                                if (callback)
                                    callback(c, val, parent, ctx);
                            });
                } else {
                    onValueChange = std::function<void(float)>(
                            [callback = this->callback, parent, &ctx, this](float val) mutable {
                                value = val;
                                value.markCleanForRender(ctx);

                                if (callback)
                                    callback(*this, val, parent, ctx);
                            });
                }

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
                        onValueChange);

                text.markCleanForRender(ctx);
                decimals.markCleanForRender(ctx);
                increment.markCleanForRender(ctx);
                value.markCleanForRender(ctx);
                min.markCleanForRender(ctx);
                max.markCleanForRender(ctx);

                assign<true>(ctx, settingData);
            } else {
                assign<false>(ctx, settingData);
            }


            return setting->get_transform();
        }

        [[nodiscard]] constexpr float getValue() const noexcept {
            return *value;
        }

        constexpr void setValue(float val) noexcept {
            value = val;
        }

        constexpr void update(RenderContext& ctx) const {
            auto& data = ctx.getChildData(key);
            auto& inputFieldView = data.getData<RenderIncrementSetting>();

            assign<false>(ctx, inputFieldView);
        }

    protected:
        template<bool created = false>
        constexpr void assign(RenderContext& parentCtx, RenderIncrementSetting& renderIncrementSetting) const {
            auto setting = renderIncrementSetting.setting;
            auto& textSetting = renderIncrementSetting.textSetting;

            RenderContext& ctx = parentCtx.getChildDataOrCreate(key).getChildContext([renderIncrementSetting]{ return renderIncrementSetting.setting->get_transform() ;});

            CRASH_UNLESS(setting);

            if (enabled.readAndClear(ctx)) {
                setting->set_enabled(*enabled);
            }

            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            // TODO: Interactable

            if constexpr(!created) {

                if (text.readAndClear(ctx)) {
                    if (!textSetting)
                        textSetting = setting->template GetComponentInChildren<TMPro::TextMeshProUGUI *>();

                    CRASH_UNLESS(textSetting);
                    textSetting->set_text(il2cpp_utils::newcsstr(*text));
                }
                if (decimals.readAndClear(ctx)) {
                    setting->Decimals = *decimals;
                }
                if (increment.readAndClear(ctx)) {
                    setting->Increment = *increment;
                }
                if (max.readAndClear(ctx)) {
                    setting->MaxValue = max.getData().value_or(0);
                    setting->HasMax = static_cast<bool>(max.getData());
                }
                if (min.readAndClear(ctx)) {
                    setting->MinValue = min.getData().value_or(0);
                    setting->HasMin = static_cast<bool>(min.getData());
                }

                if (value.readAndClear(ctx)) {
                    setting->CurrentValue = *value;
                }
            }
        }

    };

    using IncrementSetting = BasicIncrementSetting<true>;

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsIncrementSetting = ConfigUtilsSetting<float, IncrementSetting>;
#endif
    static_assert(renderable<IncrementSetting>);
}
