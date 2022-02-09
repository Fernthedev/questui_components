#pragma once

#include "shared/context.hpp"
#include "shared/state.hpp"

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
        const UnityEngine::Vector2 sizeDelta;
        const UnityEngine::Vector2 anchoredPosition;
        RenderHeldData<bool> enabled;
        RenderHeldData<UnityEngine::Sprite*> sprite;
        const Key key;

        Image(UnityEngine::Sprite* spr, UnityEngine::Vector2 sd, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool enabled_ = true)
            : sizeDelta(sd), anchoredPosition(anch), enabled(enabled_), sprite(spr) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& image = data.getData<HMUI::ImageView*>();
            if (!image) {
                image = QuestUI::BeatSaberUI::CreateImage(&ctx.parentTransform, *sprite, anchoredPosition, sizeDelta);
                assign<true>(ctx, image);
            } else {
                assign<false>(ctx, image);
            }
            return image->get_transform();
        }

    protected:
        template<bool created = false>
        void assign(RenderContext& parentCtx, HMUI::ImageView* image) {
            CRASH_UNLESS(image);

            RenderContext& ctx = parentCtx.getChildDataOrCreate(key).getChildContext([image]{ return image->get_transform() ;});

            if (enabled.readAndClear(ctx)) {
                image->set_enabled(*enabled);
            }
            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (!created) {
                if (sprite.readAndClear(ctx)) {
                    image->set_sprite(*sprite);
                }
            }
        }
    };
    static_assert(renderable<Image>);
}
