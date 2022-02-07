#pragma once

#include "UnityEngine/Vector2.hpp"

#include "shared/context.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include <string>
#include <string_view>
#include <functional>

#include "BaseSetting.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Toggle.hpp"

namespace QUC {

    template<renderable ToggleText = Text>
    struct BasicToggleSetting {
        using OnCallback = std::function<void(BasicToggleSetting&, bool, UnityEngine::Transform*, RenderContext& ctx)>;
        const OnCallback callback;


        // initialized at render
        ToggleText text;
        HeldData<bool> enabled;
        HeldData<bool> value;
        HeldData<bool> interactable;
        const std::optional<UnityEngine::Vector2> anchoredPosition;
        const Key key;

        [[nodiscard]] bool getValue() const {
            return *value;
        }

        void setValue(bool val) {
            value = val;
        }

        template<class F = OnCallback>
        BasicToggleSetting(Text const& txt, F&& callable, bool currentValue = false, bool enabled_ = true, bool interact = true, std::optional<UnityEngine::Vector2> anch = std::nullopt)
                : text(txt), callback(callable), enabled(enabled_), value(currentValue), interactable(interact), anchoredPosition(anch) {}

        template<class F = OnCallback>
        BasicToggleSetting(std::string_view txt, F&& callable, bool currentValue = false, bool enabled_ = true, bool interact = true, std::optional<UnityEngine::Vector2> anch = std::nullopt)
                : text(txt), callback(callable), enabled(enabled_), value(currentValue), interactable(interact), anchoredPosition(anch) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& toggle = data.getData<UnityEngine::UI::Toggle*>();
            auto& toggleText = ctx.getChildData(text.key).template getData<TMPro::TextMeshProUGUI*>();

            auto parent = &ctx.parentTransform;
            if (!toggle) {
                auto const &usableText = *text.text;

                auto cbk = [this, callback = this->callback, parent, &ctx](bool val) {
                    value = val;
                    value.clear();
                    if (callback)
                        callback(*this, val, parent, ctx);
                };
                if (anchoredPosition) {
                    toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, *value, *anchoredPosition, cbk);
                } else {
                    toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, *value, cbk);
                }

                auto nameTextTransform = CRASH_UNLESS(toggle->get_transform()->get_parent()->Find(il2cpp_utils::newcsstr("NameText")));
                auto nameText = nameTextTransform->get_gameObject();
                CRASH_UNLESS(nameText);
                toggleText = nameText->template GetComponent<TMPro::TextMeshProUGUI *>();

                // if text was not created
                text.text.markCleanForRender(ctx);

                // first render
                assign<true>(toggle, ctx);
            } else {
                // update
                CRASH_UNLESS(toggleText);
                assign<false>(toggle, ctx);
            }
            return toggle->get_transform();
        }

        void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& toggle = data.getData<UnityEngine::UI::Toggle*>();

            assign<false>(toggle, ctx);
        }

    protected:
        template<bool created = false>
        void assign(UnityEngine::UI::Toggle* toggle, RenderContext& ctx) {
            CRASH_UNLESS(toggle);
            if (enabled) {
                toggle->set_enabled(*enabled);
                enabled.clear();
            }

            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (created) {
                toggle->set_interactable(*interactable);
                interactable.clear();
            } else if (interactable) {
                toggle->set_interactable(*interactable);
                interactable.clear();
            }

            if constexpr (!created) {
                // Only set these properties if we did NOT JUST create the text.
                detail::renderSingle(text, ctx);

                if (value) {
                    toggle->set_isOn(*value);
                    value.clear();
                }
            }
        }
    };
    using ToggleSetting = BasicToggleSetting<Text>;

    static_assert(renderable<ToggleSetting>);
    static_assert(renderable_return<ToggleSetting, UnityEngine::Transform*>);
    static_assert(IsConfigType<ToggleSetting, bool>);

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsToggleSetting = ConfigUtilsSetting<bool, ToggleSetting>;
#endif
}
