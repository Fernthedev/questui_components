#pragma once

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/InputFieldView.hpp"

#include <string>

namespace QUC {
    struct StringSetting {
        using OnCallback = std::function<void(StringSetting*, std::string const&, UnityEngine::Transform*)>;
        HeldData<std::string> text;
        OnCallback callback;
        HeldData<bool> enabled;
        HeldData<bool> interactable;
        HeldData<std::string> value;
        UnityEngine::Vector2 anchoredPosition;
        UnityEngine::Vector3 keyboardPositionOffset;

        template<class F>
        constexpr StringSetting(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, std::string_view currentValue = "", UnityEngine::Vector2 anch = {}, UnityEngine::Vector3 offt = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), anchoredPosition(anch), keyboardPositionOffset(offt) {}

        auto render(RenderContext& ctx) {
            // TODO: Cache this properly
            auto parent = &ctx.parentTransform;
            if (!inputFieldView) {
                inputFieldView = QuestUI::BeatSaberUI::CreateStringSetting(parent, *text, *value, anchoredPosition,
                                                                           keyboardPositionOffset,
                                                                           [this, parent](std::string_view val) {
                                                                               value = val;
                                                                               value.clear();
                                                                               callback(this, value.getData(), parent);
                                                                           });
                assign<true>();
            } else {
                assign<false>();
            }

            return inputFieldView->get_transform();
        }

        inline void update() {
            assign<false>();
        }

    private:
        template<bool created>
        void assign() {
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

        WeakPtrGO<HMUI::InputFieldView> inputFieldView;
    };
    static_assert(renderable<StringSetting>);
}
