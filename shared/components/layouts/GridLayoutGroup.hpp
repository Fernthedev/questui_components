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
            auto render(RenderContext& ctx) {
                // TODO: Cache this properly
                auto obj = QuestUI::BeatSaberUI::CreateGridLayoutGroup(&ctx.parentTransform)->get_transform();
                RenderContext ctx2(obj);
                Container<TArgs...>::render(ctx2);
                return obj;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto GridLayoutGroup(TArgs&&... args) {
        return detail::GridLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}
