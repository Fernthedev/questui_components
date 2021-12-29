#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include <string>
#include <string_view>
#include <functional>

#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Toggle.hpp"

namespace QUC {
    struct ToggleSetting {
        struct ToggleText : public Text {
        public:
            ToggleText(Text const& text) : Text(text) {}

            // Copy with existing TMP
            ToggleText(TMPro::TextMeshProUGUI* textComp, Text const& text) : Text(textComp, text) {}

            // Grab values from tmp
            explicit ToggleText(TMPro::TextMeshProUGUI* textComp) : Text(textComp) {}
        };

        using OnCallback = std::function<void(ToggleSetting*, bool, UnityEngine::Transform*)>;
        OnCallback callback;
        bool enabled;
        bool interactable;
        bool value;
        // initialized at render
        std::optional<ToggleText> text;
        UnityEngine::Vector2 anchoredPosition;


        template<class F>
        ToggleSetting(Text const& txt, F&& callable, bool enabled_ = true, bool interact = true, bool currentValue = false, UnityEngine::Vector2 anch = {})
            : text(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), anchoredPosition(anch) {}

        template<class F>
        ToggleSetting(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, bool currentValue = false, UnityEngine::Vector2 anch = {})
                : str(txt), callback(callable), enabled(enabled_), interactable(interact), value(currentValue), anchoredPosition(anch) {}

        auto render(RenderContext& ctx) {
            auto parent = &ctx.parentTransform;
            if (!toggle) {
                auto const& usableText = text ? text->text : *str;

                toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, value, anchoredPosition,
                                                            [this, parent](bool val) {
                                                                callback(this, val, parent);
                                                            });
                auto nameTextTransform = CRASH_UNLESS(toggle->get_transform()->get_parent()->Find(il2cpp_utils::createcsstr("NameText")));
                auto nameText = nameTextTransform->get_gameObject();
                CRASH_UNLESS(nameText);
                textParent = nameTextTransform->get_parent();
                auto toggleText = nameText->GetComponent<TMPro::TextMeshProUGUI*>();

                if (text) {
                    text = ToggleText(toggleText, *text);
                } else {
                    text = ToggleText(toggleText);
                }
                // first render
                assign<true>();

                auto textCtx = RenderContext(textParent);
                text->render(textCtx);
            } else {
                // update
                assign<false>();
                auto textCtx = RenderContext(textParent);
                text->render(textCtx);
            }
            return toggle->get_transform();
        }

    protected:
        template<bool created = false>
        void assign() {
            toggle->set_enabled(enabled);
            if (!enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }
            auto rectTransform = toggle->GetComponent<UnityEngine::RectTransform*>();
            if constexpr (!created) {
                // Only set these properties if we did NOT JUST create the text.
                toggle->set_isOn(value);
                rectTransform->set_anchoredPosition(anchoredPosition);
            }
            toggle->set_interactable(interactable);
        }

        // TODO: Somehow not require this
        std::optional<std::string> str = std::nullopt;

        WeakPtrGO<UnityEngine::UI::Toggle> toggle;
        UnityEngine::Transform* textParent = nullptr;
    };
    static_assert(renderable<ToggleSetting>);
    static_assert(renderable_return<ToggleSetting, UnityEngine::Transform*>);
}
