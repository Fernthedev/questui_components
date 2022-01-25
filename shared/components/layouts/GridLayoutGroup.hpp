#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"
#include "UnityEngine/UI/GridLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct GridLayoutGroup : Container<TArgs...> {
            static_assert(renderable<GridLayoutGroup<TArgs...>>);
            GridLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}

            const Key key;

            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& gridLayoutGroup = data.getData<UnityEngine::UI::GridLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!gridLayoutGroup) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    gridLayoutGroup = QuestUI::BeatSaberUI::CreateGridLayoutGroup(&parent);
                }

                RenderContext& childrenCtx = ctx.getChildContext<UnityEngine::UI::GridLayoutGroup>(key, [gridLayoutGroup]() {
                    return gridLayoutGroup->get_transform();
                });
                detail::Container<TArgs...>::render(childrenCtx, data);
                return &childrenCtx.parentTransform;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto GridLayoutGroup(TArgs&&... args) {
        return detail::GridLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}
