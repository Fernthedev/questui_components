#pragma once

#include "shared/context.hpp"
#include "settings/ToggleSetting.hpp"
#include "Image.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include <string>
#include <functional>
#include <utility>

namespace QUC
{

    // TODO: Somehow this causes game buttons to be wide. How to fix?
    struct ModifierToggle : public ToggleSetting
    {
    public:
        Image image;

        template <class F = OnCallback>
        ModifierToggle(Text const &txt, F &&callable, bool currentValue, Image image = Image(nullptr, {0, 0}))
            : ToggleSetting(txt,
                            callable,
                            currentValue),
              image(std::move(image)) {}

        template <class F = OnCallback>
        ModifierToggle(std::string_view txt, F &&callable, bool currentValue, Image image = Image(nullptr, {0, 0}))
            : ToggleSetting(txt,
                            callable,
                            currentValue),
              image(std::move(image)) {}

        template <class F = OnCallback>
        ModifierToggle(Text const &txt, F &&callable, bool currentValue,
                       bool enabled_ = true, bool interact = true,
                       std::optional<UnityEngine::Vector2> anch = std::nullopt, Image image = Image(nullptr, {0, 0}))
            : ToggleSetting(txt,
                            callable,
                            currentValue,
                            enabled_,
                            interact,
                            anch),
              image(std::move(image)) {}

        template <class F = OnCallback>
        ModifierToggle(std::string_view const &txt, F &&callable, bool currentValue,
                       bool enabled_ = true, bool interact = true,
                       std::optional<UnityEngine::Vector2> anch = std::nullopt, Image image = Image(nullptr, {0, 0}))
            : ToggleSetting(txt,
                            callable,
                            currentValue,
                            enabled_,
                            interact,
                            anch),
              image(std::move(image)) {}

        UnityEngine::Transform *render(RenderContext &ctx, RenderContextChildData &data)
        {
            auto &toggle = ctx.getChildData(ToggleSetting::key).getData<UnityEngine::UI::Toggle *>();
            auto &toggleText = ctx.getChildData(text.key).getData<TMPro::TextMeshProUGUI *>();
            auto &imageView = ctx.getChildData(image.key).getData<HMUI::ImageView *>();

            if (!toggle)
            {
                auto parent = &ctx.parentTransform;
                auto const &usableText = *text.text;

                auto cbk = [this, callback = this->callback, parent, &ctx](bool val)
                {
                    toggleButton.value = val;
                    toggleButton.value.clear();
                    if (callback)
                        callback(*this, val, parent, ctx);
                };
                if (anchoredPosition)
                {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, *toggleButton.value,
                                                                        *image.sprite, cbk, *anchoredPosition);
                }
                else
                {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, *toggleButton.value,
                                                                        *image.sprite, cbk);
                }
                text.text.clear();
                image.sprite.clear();
                toggleButton.value.clear();

                auto toggleTransform = toggle->get_transform();

                imageView = toggleTransform->Find(il2cpp_utils::newcsstr("Icon"))->GetComponent<HMUI::ImageView *>();
                toggleText = toggleTransform->Find(il2cpp_utils::newcsstr("Name"))->GetComponent<TMPro::TextMeshProUGUI *>();
            }

            return ToggleSetting::render(ctx, data);
        }
    };

    static_assert(renderable<ModifierToggle>);

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsModifierToggleSetting = ConfigUtilsSetting<bool, ModifierToggle>;
#endif
}