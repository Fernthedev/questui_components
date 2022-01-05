#pragma once

#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/TextPageScrollView.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

namespace QUC {

    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    struct ScrollableContainer : detail::Container<TArgs...> {
        ScrollableContainer(TArgs... args) : detail::Container<TArgs...>(args...) {}

        const Key key;

        UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            auto& scrollContainer = data.getData<UnityEngine::GameObject*>();
            auto &parent = ctx.parentTransform;
            if (!scrollContainer) {
                // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                scrollContainer = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(&parent);
            }

            RenderContext& childrenCtx = ctx.getChildContext<HMUI::ScrollView>(key, [scrollContainer]() {
                return scrollContainer->get_transform();
            });
            detail::Container<TArgs...>::render(childrenCtx, data);
            return &childrenCtx.parentTransform;
        }
    };

    static_assert(renderable<ScrollableContainer<Text>>);

}