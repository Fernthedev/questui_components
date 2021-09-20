#pragma once

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include <string>
#include <functional>

namespace UnityEngine::UI {
    class Image;
}

namespace QUC {
    struct Button {
        std::string text;
        bool enabled;
        bool interactable;
        UnityEngine::UI::Image* image;
        UnityEngine::Vector2 anchoredPosition;
        UnityEngine::Vector2 sizeDelta;
        std::string buttonTemplate;

        template<class F>
        Button(std::string_view txt, F&& callable, bool enabled_ = true, bool interact = true, UnityEngine::UI::Image* img = nullptr, UnityEngine::Vector2 anch = {}, UnityEngine::Vector2 sz = {}, std::string buttonTemplate_ = "")
            : text(txt), enabled(enabled_), interactable(interact), image(img), anchoredPosition(anch), sizeDelta(sz), buttonTemplate(buttonTemplate_), click(callable) {}
        auto render(RenderContext& ctx) {
            // TODO: Cache this for avoiding tree reparses
            auto parent = &ctx.parentTransform;
            std::function<void()> callback = [this, parent]{
                click(this, parent);
            };
            auto res = QuestUI::BeatSaberUI::CreateUIButton(parent, text, buttonTemplate, anchoredPosition, sizeDelta, callback);
            res->set_enabled(enabled);
            if (!enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }
            res->set_interactable(interactable);
            // auto txt = res->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
            if (image) {
                res->set_image(image);
            }
        }
        private:
        std::function<void(Button* button, UnityEngine::Transform*)> click;
    };
    static_assert(renderable<Button>);
}
