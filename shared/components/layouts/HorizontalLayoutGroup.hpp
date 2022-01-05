#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct HorizontalLayoutGroup : Container<TArgs...> {
            HorizontalLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}

            const Key key;

            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& horizontalLayout = data.getData<UnityEngine::UI::HorizontalLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!horizontalLayout) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    horizontalLayout = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(&parent);
                }

                RenderContext& childrenCtx = ctx.getChildContext<UnityEngine::UI::HorizontalLayoutGroup>(key, [horizontalLayout]() {
                    return horizontalLayout->get_transform();
                });
                detail::Container<TArgs...>::render(childrenCtx, data);
                return &childrenCtx.parentTransform;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto HorizontalLayoutGroup(TArgs&&... args) {
        return detail::HorizontalLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}
