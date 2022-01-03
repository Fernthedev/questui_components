#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "shared/unity/WeakPtrGO.hpp"
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
            HeldData<bool> value;
            HeldData<bool> interactable;

            constexpr ToggleButton(bool value, bool interactable) : value(value), interactable(interactable) {}

        protected:
            template<bool created = false>
            void assign(UnityEngine::UI::Toggle* toggle) {
                if constexpr (!created) {
                    // Only set these properties if we did NOT JUST create the text.
                    if (value) {
                        toggle->set_isOn(*value);
                        value.clear();
                    }
                }
                if constexpr (created) {
                    toggle->set_interactable(*interactable);
                    interactable.clear();
                } else if (interactable) {
                    toggle->set_interactable(*interactable);
                    interactable.clear();
                }
            }

            friend class ToggleSetting;
        };

        struct ToggleText : public Text {
        public:
            ToggleText() = default;

            ToggleText(ToggleText const& text) = default;

            ToggleText(Text const& text) : Text(text) {}

            // Grab values from tmp
            explicit ToggleText(TMPro::TextMeshProUGUI* textComp) : Text(textComp) {}

            ~ToggleText() = default;

        protected:
            template<bool created = false>
            void assign(UnityEngine::UI::Toggle* toggle) {
                Text::assign<created>();
            }

            friend class ToggleSetting;
        };

        using OnCallback = std::function<void(ToggleSetting*, bool, UnityEngine::Transform*)>;
        OnCallback callback; // TODO: Const-ify
        HeldData<bool> enabled;

        // initialized at render
        std::optional<ToggleText> text;
        ToggleButton toggleButton;
        UnityEngine::Vector2 anchoredPosition; // TODO: Const-ify


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
                                                                toggleButton.value = val;
                                                                toggleButton.value.clear();
                                                                callback(this, val, parent);
                                                            });
                auto nameTextTransform = CRASH_UNLESS(toggle->get_transform()->get_parent()->Find(il2cpp_utils::newcsstr("NameText")));
                auto nameText = nameTextTransform->get_gameObject();
                CRASH_UNLESS(nameText);
                auto textParent = nameTextTransform->get_parent();
                auto toggleText = nameText->GetComponent<TMPro::TextMeshProUGUI *>();

                auto rectTransform = toggle->GetComponent<UnityEngine::RectTransform*>();
                rectTransform->set_anchoredPosition(anchoredPosition);

                // if text was created
                if (text) {
                    text->textComp = toggleText;
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
            CRASH_UNLESS(toggle);
            if (enabled) {
                toggle->set_enabled(*enabled);
                enabled.clear();
            }

            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (!created) {
                // Only set these properties if we did NOT JUST create the text.
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

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsToggleSetting = ConfigUtilsSetting<bool, ToggleSetting>;
#endif
}
