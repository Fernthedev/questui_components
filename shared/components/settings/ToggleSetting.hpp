#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"
#include "shared/unity/WeakPtrGO.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include <string>
#include <string_view>
#include <functional>

#include "BaseSetting.hpp"

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
            void assign(TMPro::TextMeshProUGUI* textComp) {
                Text::assign<created>(textComp);
            }

            friend class ToggleSetting;
        };

        using OnCallback = std::function<void(ToggleSetting&, bool, UnityEngine::Transform*, RenderContext& ctx)>;
        const OnCallback callback;
        HeldData<bool> enabled;

        // initialized at render
        std::optional<ToggleText> text;
        ToggleButton toggleButton;
        const std::optional<UnityEngine::Vector2> anchoredPosition;
        const Key key;



        template<class F = OnCallback>
        ToggleSetting(Text const& txt, F&& callable, bool currentValue = false, bool enabled_ = true, bool interact = true, std::optional<UnityEngine::Vector2> anch = std::nullopt)
            : text(txt), callback(callable), enabled(enabled_), toggleButton(currentValue, interact), anchoredPosition(anch) {}

        template<class F = OnCallback>
        ToggleSetting(std::string_view txt, F&& callable, bool currentValue = false, bool enabled_ = true, bool interact = true, std::optional<UnityEngine::Vector2> anch = std::nullopt)
                : str(txt), callback(callable), enabled(enabled_), toggleButton(currentValue, interact), anchoredPosition(anch) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& toggle = data.getData<UnityEngine::UI::Toggle*>();
            auto& toggleText = ctx.getChildData(text->key).getData<TMPro::TextMeshProUGUI*>();

            auto parent = &ctx.parentTransform;
            if (!toggle) {
                auto const &usableText = text ? text->text : *str;

                auto cbk = [this, callback = this->callback, parent, &ctx](bool val) {
                    toggleButton.value = val;
                    toggleButton.value.clear();
                    if (callback)
                        callback(*this, val, parent, ctx);
                };
                if (anchoredPosition) {
                    toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, *toggleButton.value, *anchoredPosition, cbk);
                } else {
                    toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, *toggleButton.value, cbk);
                }

                auto nameTextTransform = CRASH_UNLESS(toggle->get_transform()->get_parent()->Find(il2cpp_utils::newcsstr("NameText")));
                auto nameText = nameTextTransform->get_gameObject();
                CRASH_UNLESS(nameText);
                toggleText = nameText->GetComponent<TMPro::TextMeshProUGUI *>();

                // if text was not created
                if (!text) {
                    text.emplace(Text());
                    text->copyFrom(toggleText, ctx);
                }
                // first render
                assign<true>(toggle, toggleText);

                text->render(ctx, ctx.getChildData(text->key));
            } else {
                // update
                CRASH_UNLESS(toggleText);
                assign<false>(toggle, toggleText);
            }
            return toggle->get_transform();
        }

        void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& toggle = data.getData<UnityEngine::UI::Toggle*>();
            auto& cachedToggleText = ctx.getChildData(text->key).getData<TMPro::TextMeshProUGUI*>();

            assign<false>(toggle, cachedToggleText);
        }

    protected:
        template<bool created = false>
        void assign(UnityEngine::UI::Toggle* toggle, TMPro::TextMeshProUGUI* toggleText) {
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
                text->assign<created>(toggleText);
                toggleButton.assign<created>(toggle);
            }
        }

        // TODO: Somehow not require this
        std::optional<std::string> str = std::nullopt;
    };
    static_assert(renderable<ToggleSetting>);
    static_assert(renderable_return<ToggleSetting, UnityEngine::Transform*>);

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsToggleSetting = ConfigUtilsSetting<bool, ToggleSetting>;
#endif
}
