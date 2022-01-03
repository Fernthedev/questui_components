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

        Image(UnityEngine::Sprite* spr, UnityEngine::Vector2 sd, UnityEngine::Vector2 anch = {0.0f, 0.0f}, bool enabled_ = true)
            : sizeDelta(sd), anchoredPosition(anch), enabled(enabled_), sprite(spr) {}
        
        auto render(RenderContext& ctx) {
            if (!image) {
                image = QuestUI::BeatSaberUI::CreateImage(&ctx.parentTransform, *sprite, anchoredPosition, sizeDelta);
                assign<true>();
            } else {
                assign<false>();
            }
            return image->get_transform();
        }

        void update() {
            assign<false>();
        }

    protected:
        template<bool created = false>
        void assign() {
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

        WeakPtrGO<HMUI::ImageView> image;
    };
    static_assert(renderable<Image>);
}
