#pragma once

#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct VerticalLayoutGroup : Container<TArgs...> {
            const Key key;

            VerticalLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}

            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& viewLayout = data.getData<UnityEngine::UI::VerticalLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!viewLayout) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    viewLayout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(&parent);
                }

                RenderContext& childrenCtx = ctx.getChildContext<UnityEngine::UI::VerticalLayoutGroup>(key, [viewLayout]() {
                    return viewLayout->get_transform();
                });
                detail::Container<TArgs...>::render(childrenCtx, data);
                return &childrenCtx.parentTransform;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto VerticalLayoutGroup(TArgs&&... args) {
        return detail::VerticalLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}