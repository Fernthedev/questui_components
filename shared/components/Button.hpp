#pragma once

#include "shared/context.hpp"
#include "shared/state.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Button.hpp"

#include "Text.hpp"

#include <string>
#include <functional>
#include <utility>

// https://github.com/darknight1050/questui/blob/7af11f2112c648290871a6ed73bcfbda05ef0dee/src/BeatSaberUI.cpp#L71
#define DEFAULT_BUTTONTEMPLATE "PracticeButton"

namespace UnityEngine::UI {
    class Image;
}

namespace QUC {

    template <renderable ButtonText = Text>
    struct BasicButton {
    private:
        struct RenderButtonData {
            UnityEngine::UI::Button* button;
            TMPro::TextMeshProUGUI* buttonText;
        };
    public:

        Text text;
        HeldData<bool> enabled;
        HeldData<bool> interactable;
        HeldData<UnityEngine::UI::Image*> image;
        const std::optional<UnityEngine::Vector2> anchoredPosition;
        const std::optional<UnityEngine::Vector2> sizeDelta;
        const std::string buttonTemplate;

        const Key key;

        template<class F>
        BasicButton(ButtonText const& txt, F&& callable, bool enabled_ = true, bool interact = true, UnityEngine::UI::Image* img = nullptr, std::optional<UnityEngine::Vector2> anch = std::nullopt, std::optional<UnityEngine::Vector2> sz = std::nullopt, std::string buttonTemplate_ = DEFAULT_BUTTONTEMPLATE)
            : text(txt), enabled(enabled_), interactable(interact), image(img), anchoredPosition(anch), sizeDelta(sz), buttonTemplate(std::move(buttonTemplate_)), click(callable) {}

        template<class F>
        BasicButton(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, UnityEngine::UI::Image* img = nullptr, std::optional<UnityEngine::Vector2> anch = std::nullopt, std::optional<UnityEngine::Vector2> sz = std::nullopt, std::string buttonTemplate_ = DEFAULT_BUTTONTEMPLATE)
                : text(txt), enabled(enabled_), interactable(interact), image(img), anchoredPosition(anch), sizeDelta(sz), buttonTemplate(std::move(buttonTemplate_)), click(callable) {}


        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& buttonData = data.getData<RenderButtonData>();
            auto* parent = &ctx.parentTransform;

            auto& button = buttonData.button;
            if (!button) {
                std::function<void()> callback = [this, callback = this->click, parent, &ctx]()mutable{
                    if (click)
                        click(*this, parent, ctx);
                };

                if (anchoredPosition) {
                    if (sizeDelta) {
                        button = QuestUI::BeatSaberUI::CreateUIButton(parent, *text.text, buttonTemplate, *anchoredPosition,
                                                                      *sizeDelta, callback);
                    } else {
                        button = QuestUI::BeatSaberUI::CreateUIButton(parent, *text.text, buttonTemplate, *anchoredPosition, callback);
                    }
                } else {
                    button = QuestUI::BeatSaberUI::CreateUIButton(parent, *text.text, buttonTemplate, callback);
                }

                text.text.markCleanForRender(ctx);

                auto& buttonText = ctx.getChildDataOrCreate(text.key).getData<TMPro::TextMeshProUGUI*>();
                buttonText = button->template GetComponentInChildren<TMPro::TextMeshProUGUI *>();

                assign<true>(ctx, buttonData);
            } else {
                assign<false>(ctx, buttonData);
            }

            return button->get_transform();
        }

        void update(RenderContext& ctx) {
            auto& data = ctx.getChildData(key);
            auto& buttonData = data.getData<RenderButtonData>();

            assign<false>(ctx, buttonData);
        }

        [[nodiscard]] BasicButton clone() const {
            BasicButton clone(*this);

            return clone;
        }

    protected:
        template<bool created = false>
        void assign(RenderContext& ctx, RenderButtonData& buttonData) {
            auto& button = buttonData.button;
            auto& buttonText = buttonData.buttonText;

            CRASH_UNLESS(button);
            if (enabled) {
                button->set_enabled(*enabled);
                enabled.clear();
            }
            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (created) {
                button->set_interactable(*interactable);
                interactable.clear();
            } else if (interactable) {
                button->set_interactable(*interactable);
                interactable.clear();
            }

            if constexpr (!created) {
                detail::renderSingle(text, ctx);

                if (image) {
                    button->set_image(*image);
                    image.clear();
                }
            }
        }
    private:
        std::function<void(BasicButton& button, UnityEngine::Transform* transform, RenderContext& ctx)> click;
    };
    using Button = BasicButton<Text>;

    static_assert(renderable<Button>);
    static_assert(cloneable<Button>);
}
