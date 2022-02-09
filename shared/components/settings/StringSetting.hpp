#pragma once

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"

#include "BaseSetting.hpp"
#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/InputFieldView.hpp"

#include <string>

namespace QUC {
    struct StringSetting {
        using OnCallback = std::function<void(StringSetting&, std::string const&, UnityEngine::Transform*, RenderContext& ctx)>;
        RenderHeldData<std::string> text;
        OnCallback callback;
        RenderHeldData<bool> enabled;
        RenderHeldData<bool> interactable;
        RenderHeldData<std::string> value;
        const UnityEngine::Vector2 anchoredPosition;
        const UnityEngine::Vector3 keyboardPositionOffset;

        const Key key;

        template<class F>
        constexpr StringSetting(std::string_view txt, F&& callable, std::string_view currentValue = "", bool enabled_ = true, bool interact = true, UnityEngine::Vector2 anch = {}, UnityEngine::Vector3 offt = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), anchoredPosition(anch), keyboardPositionOffset(offt) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& inputFieldView = data.getData<HMUI::InputFieldView*>();
            // TODO: Cache this properly
            auto parent = &ctx.parentTransform;
            if (!inputFieldView) {
                auto cbk = [callback = this->callback, parent, &ctx, this](StringW val)mutable {
                    value = static_cast<std::string>(val);
                    value.markCleanForRender(ctx);
                    if (callback)
                        callback(*this, value.getData(), parent, ctx);
                };
                inputFieldView = QuestUI::BeatSaberUI::CreateStringSetting(parent, *text, *value, anchoredPosition,
                                                                           keyboardPositionOffset,
                                                                           cbk);
                text.markCleanForRender(ctx);
                value.markCleanForRender(ctx);

                assign<true>(ctx, inputFieldView);
            } else {
                assign<false>(ctx, inputFieldView);
            }

            return inputFieldView->get_transform();
        }

        [[nodiscard]] std::string const& getValue() const {
            return *value;
        }

        void setValue(std::string_view val) noexcept {
            value = val;
        }

        void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& inputFieldView = data.getData<HMUI::InputFieldView*>();

            assign<false>(ctx, inputFieldView);
        }

    private:
        template<bool created>
        void assign(RenderContext& parentCtx, HMUI::InputFieldView* inputFieldView) {
            CRASH_UNLESS(inputFieldView);

            RenderContext& ctx = parentCtx.getChildDataOrCreate(key).getChildContext([inputFieldView]{ return inputFieldView->get_transform() ;});

            if (enabled.readAndClear(ctx)) {
                inputFieldView->set_enabled(*enabled);
            }

            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (created) {
                inputFieldView->set_interactable(*interactable);
                interactable.markCleanForRender(ctx);
            }

            if constexpr (!created) {
                if (interactable.readAndClear(ctx)) {
                    inputFieldView->set_interactable(*interactable);
                }

                if (text.readAndClear(ctx)) {
                    // TODO: cache this
                    auto txt = inputFieldView->dyn__placeholderText()->GetComponent<TMPro::TextMeshProUGUI *>();
                    CRASH_UNLESS(txt);
                    txt->set_text(il2cpp_utils::newcsstr(*text));
                }

                if (value.readAndClear(ctx)) {
                    inputFieldView->SetText(il2cpp_utils::newcsstr(*value));
                }
            }
        }
    };
    static_assert(renderable<StringSetting>);
    static_assert(IsConfigType<StringSetting, std::string>);

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsStringSetting = ConfigUtilsSetting<std::string, StringSetting>;
#endif


}
