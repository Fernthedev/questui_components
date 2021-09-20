#pragma once

#include "shared/context.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/Vector2.hpp"

namespace UnityEngine {
    class Sprite;
    namespace UI {
        class Image;
    }
}

namespace QUC {
    struct Image {
        UnityEngine::Vector2 sizeDelta;
        UnityEngine::Vector2 anchoredPosition;
        bool enabled;
        UnityEngine::Sprite* sprite;

        Image(UnityEngine::Sprite* spr, UnityEngine::Vector2 sd, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool enabled_ = true)
            : sizeDelta(sd), anchoredPosition(anch), enabled(enabled_), sprite(spr) {}
        
        auto render(RenderContext& ctx) {
            // TODO: Add proper tree recaching on parent context.
            auto res = QuestUI::BeatSaberUI::CreateImage(&ctx.parentTransform, sprite, anchoredPosition, sizeDelta);
            res->set_enabled(enabled);
            return res->get_transform();
        }
    };
    static_assert(renderable<Image>);
}
