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
        HeldData<std::string> text;
        OnCallback callback;
        HeldData<bool> enabled;
        HeldData<bool> interactable;
        HeldData<std::string> value;
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
                auto cbk = [callback = this->callback, parent, &ctx, this](std::string_view val)mutable {
                    value = val;
                    value.clear();
                    if (callback)
                        callback(*this, value.getData(), parent, ctx);
                };
                inputFieldView = QuestUI::BeatSaberUI::CreateStringSetting(parent, *text, *value, anchoredPosition,
                                                                           keyboardPositionOffset,
                                                                           cbk);
                assign<true>(inputFieldView);
            } else {
                assign<false>(inputFieldView);
            }

            return inputFieldView->get_transform();
        }

        void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& inputFieldView = data.getData<HMUI::InputFieldView*>();

            assign<false>(inputFieldView);
        }

    private:
        template<bool created>
        void assign(HMUI::InputFieldView* inputFieldView) {
            CRASH_UNLESS(inputFieldView);
            if (enabled) {
                inputFieldView->set_enabled(*enabled);
                enabled.clear();
            }

            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (created) {
                inputFieldView->set_interactable(*interactable);
                interactable.clear();
            } else if (interactable) {
                inputFieldView->set_interactable(*interactable);
                interactable.clear();
            }

            if constexpr (!created) {
                if (text) {
                    auto txt = inputFieldView->placeholderText->GetComponent<TMPro::TextMeshProUGUI *>();
                    CRASH_UNLESS(txt);
                    txt->set_text(il2cpp_utils::newcsstr(*text));
                    text.clear();
                }

                if (value) {
                    inputFieldView->SetText(il2cpp_utils::newcsstr(*value));
                }
            }
        }
    };
    static_assert(renderable<StringSetting>);

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsStringSetting = ConfigUtilsSetting<std::string, StringSetting>;
#endif


}
