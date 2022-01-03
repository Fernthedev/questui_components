#pragma once

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include <string>
#include <functional>
#include <utility>

namespace UnityEngine::UI {
    class Image;
}

namespace QUC {
    struct Button {
        HeldData<std::string> text;
        HeldData<bool> enabled;
        HeldData<bool> interactable;
        HeldData<UnityEngine::UI::Image*> image;
        const UnityEngine::Vector2 anchoredPosition;
        const UnityEngine::Vector2 sizeDelta;
        const std::string buttonTemplate;

        template<class F>
        Button(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, UnityEngine::UI::Image* img = nullptr, UnityEngine::Vector2 anch = {}, UnityEngine::Vector2 sz = {}, std::string buttonTemplate_ = "")
            : text(txt), enabled(enabled_), interactable(interact), image(img), anchoredPosition(anch), sizeDelta(sz), buttonTemplate(std::move(buttonTemplate_)), click(callable) {}
        auto render(RenderContext& ctx) {
            // TODO: Cache this for avoiding tree reparses
            auto parent = &ctx.parentTransform;

            if (!button) {
                std::function<void()> callback = [this, parent]{
                    click(this, parent);
                };

                button = QuestUI::BeatSaberUI::CreateUIButton(parent, *text, buttonTemplate, anchoredPosition,
                                                              sizeDelta, callback);
                assign<true>();
            } else {
                assign<false>();
            }

            return button->get_transform();
        }

        void update() {
            assign<false>();
        }

        [[nodiscard]] Button clone() const {
            Button clone(*this);
            clone.button = nullptr;
            clone.buttonText = nullptr;

            return clone;
        }

    protected:
        template<bool created = false>
        void assign() {
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
                if (text) {
                    if (!buttonText)
                        buttonText = button->GetComponentInChildren<TMPro::TextMeshProUGUI *>();

                    buttonText->set_text(il2cpp_utils::newcsstr(*text));
                    text.clear();
                }
                if (image) {
                    button->set_image(*image);
                    image.clear();
                }
            }
        }

        private:
        WeakPtrGO<UnityEngine::UI::Button> button;
        WeakPtrGO<TMPro::TextMeshProUGUI> buttonText;

        std::function<void(Button* button, UnityEngine::Transform* transform)> click;
    };
    static_assert(renderable<Button>);
    static_assert(cloneable<Button>);
}
