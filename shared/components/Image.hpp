#pragma once

#include "shared/context.hpp"
#include "shared/state.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/Vector2.hpp"
#include "sombrero/shared/FastVector2.hpp"

namespace UnityEngine {
    class Sprite;
    namespace UI {
        class Image;
    }
}

namespace QUC {
    struct Image {
        RenderHeldData<Sombrero::FastVector2> sizeDelta;
        const UnityEngine::Vector2 anchoredPosition;
        RenderHeldData<bool> enabled;
        RenderHeldData<bool> preserveAspectRatio;
        RenderHeldData<UnityEngine::Sprite*> sprite;
        const Key key;

        Image(UnityEngine::Sprite* spr, UnityEngine::Vector2 sd, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool enabled_ = true)
                : sizeDelta(sd), anchoredPosition(anch), enabled(enabled_), sprite(spr) {}

        constexpr UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) const {
            auto& image = data.getData<HMUI::ImageView*>();
            if (!image) {
                image = QuestUI::BeatSaberUI::CreateImage(&ctx.parentTransform, *sprite, anchoredPosition, *sizeDelta);
                sizeDelta.markCleanForRender(ctx);
                assign<true>(ctx, image);
            } else {
                assign<false>(ctx, image);
            }
            return image->get_transform();
        }

    protected:
        template<bool created = false>
        void assign(RenderContext& parentCtx, HMUI::ImageView* image) const {
            CRASH_UNLESS(image);

            RenderContext& ctx = parentCtx.getChildDataOrCreate(key).getChildContext([image]{ return image->get_transform() ;});

            if (enabled.readAndClear(ctx)) {
                image->set_enabled(*enabled);
            }
            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if (preserveAspectRatio.readAndClear(ctx)) {
                image->set_preserveAspect(preserveAspectRatio.getData());
            }

            if constexpr (!created) {
                if (sprite.readAndClear(ctx)) {
                    image->set_sprite(*sprite);
                }

                if (sizeDelta.readAndClear(ctx)) {
                    image->get_rectTransform()->set_sizeDelta(*sizeDelta);
                }
            }
        }
    };
    static_assert(renderable<Image>);
}
