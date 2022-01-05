#pragma once

#include "shared/context.hpp"
#include "shared/state.hpp"
#include "shared/unity/WeakPtrGO.hpp"
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
        HeldData<bool> enabled;
        HeldData<UnityEngine::Sprite*> sprite;
        const Key key;

        Image(UnityEngine::Sprite* spr, UnityEngine::Vector2 sd, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool enabled_ = true)
            : sizeDelta(sd), anchoredPosition(anch), enabled(enabled_), sprite(spr) {}

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& image = data.getData<HMUI::ImageView*>();
            if (!image) {
                image = QuestUI::BeatSaberUI::CreateImage(&ctx.parentTransform, *sprite, anchoredPosition, sizeDelta);
                assign<true>(image);
            } else {
                assign<false>(image);
            }
            return image->get_transform();
        }

    protected:
        template<bool created = false>
        void assign(HMUI::ImageView* image) {
            CRASH_UNLESS(image);

            if (enabled) {
                image->set_enabled(*enabled);
                enabled.clear();
            }
            if (!*enabled) {
                // Don't bother setting anything if we aren't enabled.
                return;
            }

            if constexpr (!created) {
                if (sprite) {
                    image->set_sprite(*sprite);
                    sprite.clear();
                }
            }
        }
    };
    static_assert(renderable<Image>);
}
