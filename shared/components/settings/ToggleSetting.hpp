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

    template<renderable ToggleText = Text, bool copySelfOnCallback = true>
    struct BasicToggleSetting {
        using OnCallback = std::function<void(BasicToggleSetting&, bool, UnityEngine::Transform*, RenderContext& ctx)>;
        const OnCallback callback;


        // initialized at render
        ToggleText text;
        RenderHeldData<bool> enabled;
        RenderHeldData<bool> value;
        RenderHeldData<bool> interactable;
        const std::optional<UnityEngine::Vector2> anchoredPosition;
        const Key key;

        [[nodiscard]] constexpr bool getValue() const {
            return *value;
        }

        constexpr void setValue(bool val) noexcept {
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
                std::function<void(bool)> onToggle;

                if constexpr (copySelfOnCallback) {
                    auto c = *this;
                    onToggle = [c, callback = this->callback, parent, &ctx](bool val) mutable {
                        c.value = val;
                        c.value.markCleanForRender(ctx);
                        if (callback)
                            callback(c, val, parent, ctx);
                    };
                } else {
                    onToggle = [this, callback = this->callback, parent, &ctx](bool val) {
                        value = val;
                        value.markCleanForRender(ctx);
                        if (callback)
                            callback(*this, val, parent, ctx);
                    };
                }

                if (anchoredPosition) {
                    toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, *value, *anchoredPosition, onToggle);
                } else {
                    toggle = QuestUI::BeatSaberUI::CreateToggle(parent, usableText, *value, onToggle);
                }

                auto nameTextTransform = CRASH_UNLESS(toggle->get_transform()->get_parent()->Find(il2cpp_utils::newcsstr("NameText")));
                auto nameText = nameTextTransform->get_gameObject();
                CRASH_UNLESS(nameText);
                toggleText = nameText->template GetComponent<TMPro::TextMeshProUGUI *>();

                // if text was not created
                text.text.markCleanForRender(ctx);
                value.markCleanForRender(ctx);

                // first render
                assign<true>(toggle, ctx);
            } else {
                // update
                CRASH_UNLESS(toggleText);
                assign<false>(toggle, ctx);
            }
            return toggle->get_transform();
        }

        constexpr void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& toggle = data.getData<UnityEngine::UI::Toggle*>();

            assign<false>(toggle, ctx);
        }

    protected:
        template<bool created = false>
        constexpr void assign(UnityEngine::UI::Toggle* toggle, RenderContext& parentCtx) {
            CRASH_UNLESS(toggle);

            RenderContext& ctx = parentCtx.getChildDataOrCreate(key).getChildContext([toggle]{ return toggle->get_transform() ;});

            if (enabled.readAndClear(ctx)) {
                toggle->set_enabled(*enabled);
            }

            if (!enabled.getData()) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (created) {
                toggle->set_interactable(*interactable);
                interactable.markCleanForRender(ctx);
            }

            if constexpr (!created) {
                if (interactable.readAndClear(ctx)) {
                    toggle->set_interactable(*interactable);
                }

                // Only set these properties if we did NOT JUST create the text.
                detail::renderSingle(text, parentCtx);

                if (value.readAndClear(ctx)) {
                    toggle->set_isOn(*value);
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
