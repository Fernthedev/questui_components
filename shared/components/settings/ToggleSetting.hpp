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
        struct ToggleButton {
            bool value;
            bool interactable;

            constexpr ToggleButton(bool value, bool interactable) : value(value), interactable(interactable) {}

        protected:
            template<bool created = false>
            void assign(UnityEngine::UI::Toggle* toggle) {
                if constexpr (!created) {
                    // Only set these properties if we did NOT JUST create the text.
                    toggle->set_isOn(value);
                }
                toggle->set_interactable(interactable);
            }

            friend class ToggleSetting;
        };

        struct ToggleText : public Text {
        public:
            ToggleText(Text const& text) : Text(text) {}

            // Copy with existing TMP
            ToggleText(TMPro::TextMeshProUGUI* textComp, Text const& text) : Text(textComp, text) {}

            // Grab values from tmp
            explicit ToggleText(TMPro::TextMeshProUGUI* textComp) : Text(textComp) {}

        protected:
            template<bool created = false>
            void assign(UnityEngine::UI::Toggle* toggle) {
                Text::assign<created>();
            }

            friend class ToggleSetting;
        };

        using OnCallback = std::function<void(ToggleSetting*, bool, UnityEngine::Transform*)>;
        OnCallback callback;
        bool enabled;


        // initialized at render
        std::optional<ToggleText> text;
        ToggleButton toggleButton;
        UnityEngine::Vector2 anchoredPosition;


        template<class F>
        ToggleSetting(Text const& txt, F&& callable, bool enabled_ = true, bool interact = true, bool currentValue = false, UnityEngine::Vector2 anch = {})
            : text(txt), callback(callable), enabled(enabled_), toggleButton(currentValue, interact), anchoredPosition(anch) {}

        template<class F>
        ToggleSetting(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, bool currentValue = false, UnityEngine::Vector2 anch = {})
                : str(txt), callback(callable), enabled(enabled_), toggleButton(currentValue, interact), anchoredPosition(anch) {}

        auto render(RenderContext& ctx) {
            auto parent = &ctx.parentTransform;
            if (!toggle) {
                auto const &usableText = text ? text->text : *str;

                toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, toggleButton.value, anchoredPosition,
                                                            [this, parent](bool val) {
                                                                callback(this, val, parent);
                                                            });
                auto nameTextTransform = CRASH_UNLESS(
                        toggle->get_transform()->get_parent()->Find(il2cpp_utils::newcsstr("NameText")));
                auto nameText = nameTextTransform->get_gameObject();
                CRASH_UNLESS(nameText);
                auto textParent = nameTextTransform->get_parent();
                auto toggleText = nameText->GetComponent<TMPro::TextMeshProUGUI *>();

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
            }
            return toggle->get_transform();
        }

        void update() {
            CRASH_UNLESS(toggle);
            assign<false>();
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
                toggle->set_isOn(toggleButton.value);
                rectTransform->set_anchoredPosition(anchoredPosition);

                text->assign<created>(toggle);
                toggleButton.assign<created>(toggle);
            }
        }

        // TODO: Somehow not require this
        std::optional<std::string> str = std::nullopt;
        WeakPtrGO<UnityEngine::UI::Toggle> toggle;
    };
    static_assert(renderable<ToggleSetting>);
    static_assert(renderable_return<ToggleSetting, UnityEngine::Transform*>);
}
