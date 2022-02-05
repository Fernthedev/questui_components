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
    template<renderable ButtonText = Text>
    struct BasicModifierToggle : public BasicToggleSetting<ButtonText>
    {
    public:
        Image image;
        using OnCallback = typename BasicToggleSetting<ButtonText>::OnCallback;
        using ToggleSetting = BasicToggleSetting<ButtonText>;

        template <class F = OnCallback>
        BasicModifierToggle(ButtonText const &txt, F &&callable, bool currentValue, Image image = Image(nullptr, {0, 0}))
            : ToggleSetting(txt,
                            callable,
                            currentValue),
              image(std::move(image)) {}

        template <class F = OnCallback>
        BasicModifierToggle(std::string_view txt, F &&callable, bool currentValue, Image image = Image(nullptr, {0, 0}))
            : ToggleSetting(txt,
                            callable,
                            currentValue),
              image(std::move(image)) {}

        template <class F = OnCallback>
        BasicModifierToggle(ButtonText const &txt, F &&callable, bool currentValue,
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
        BasicModifierToggle(std::string_view const &txt, F &&callable, bool currentValue,
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
            auto &toggle = ctx.getChildData(ToggleSetting::key).template getData<UnityEngine::UI::Toggle *>();
            auto &toggleText = ctx.getChildData(ToggleSetting::text.key).template getData<TMPro::TextMeshProUGUI *>();
            auto &imageView = ctx.getChildData(image.key).getData<HMUI::ImageView *>();

            auto& toggleButton = ToggleSetting::toggleButton;

            if (!toggle)
            {
                auto parent = &ctx.parentTransform;
                auto const &usableText = *ToggleSetting::text.text;

                auto cbk = [this, callback = this->callback, parent, &ctx](bool val)
                {
                    toggleButton.value = val;
                    toggleButton.value.clear();
                    if (callback)
                        callback(*this, val, parent, ctx);
                };
                if (ToggleSetting::anchoredPosition)
                {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, *toggleButton.value,
                                                                        *image.sprite, cbk, *ToggleSetting::anchoredPosition);
                }
                else
                {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, *toggleButton.value,
                                                                        *image.sprite, cbk);
                }
                ToggleSetting::text.text.clear();
                image.sprite.clear();
                toggleButton.value.clear();

                auto toggleTransform = toggle->get_transform();

                imageView = toggleTransform->Find(il2cpp_utils::newcsstr("Icon"))->template GetComponent<HMUI::ImageView *>();
                toggleText = toggleTransform->Find(il2cpp_utils::newcsstr("Name"))->template GetComponent<TMPro::TextMeshProUGUI *>();
            }

            return ToggleSetting::render(ctx, data);
        }
    };

    using ModifierToggle = BasicModifierToggle<Text>;

    static_assert(renderable<ModifierToggle>);

#if defined(AddConfigValue) || __has_include("config-utils/shared/config-utils.hpp")
    using ConfigUtilsModifierToggleSetting = ConfigUtilsSetting<bool, ModifierToggle>;
#endif
}