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
    template<renderable ButtonText = Text, bool copySelfOnCallback = true>
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

            if (!toggle)
            {
                auto parent = &ctx.parentTransform;
                auto const &usableText = *ToggleSetting::text.text;

                std::function<void(bool)> cbk;

                if constexpr (copySelfOnCallback) {
                    auto c = *this;

                    cbk = [c, callback = this->callback, parent, &ctx](bool val) mutable {
                        c.value = val;
                        if (callback)
                            callback(c, val, parent, ctx);
                    };
                } else {
                    cbk = [this, callback = this->callback, parent, &ctx](bool val) {
                        ToggleSetting::value = val;
                        if (callback)
                            callback(*this, val, parent, ctx);
                    };
                }
                if (ToggleSetting::anchoredPosition)
                {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, ToggleSetting::value,
                                                                        *image.sprite, cbk, *ToggleSetting::anchoredPosition);
                }
                else
                {
                    toggle = QuestUI::BeatSaberUI::CreateModifierButton(parent, usableText, ToggleSetting::value,
                                                                        *image.sprite, cbk);
                }

                auto toggleTransform = toggle->get_transform();

                imageView = toggleTransform->Find(il2cpp_utils::newcsstr("Icon"))->template GetComponent<HMUI::ImageView *>();
                toggleText = toggleTransform->Find(il2cpp_utils::newcsstr("Name"))->template GetComponent<TMPro::TextMeshProUGUI *>();

                ToggleSetting::text.text.markCleanForRender(ctx);

                ToggleSetting::text.fontSize = toggleText->get_fontSize();
                ToggleSetting::text.fontSize.markCleanForRender(ctx);

                ToggleSetting::text.italic.markCleanForRender(ctx);

                ToggleSetting::text.color = toggleText->get_color();
                ToggleSetting::text.color.markCleanForRender(ctx);

                image.sprite.markCleanForRender(ctx);
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