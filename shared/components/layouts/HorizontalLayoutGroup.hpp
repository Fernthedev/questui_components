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
            auto render(RenderContext& ctx) {
                // TODO: Cache this properly
                auto obj = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(&ctx.parentTransform)->get_transform();
                RenderContext ctx2(obj);
                Container<TArgs...>::render(ctx2);
                return obj;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto HorizontalLayoutGroup(TArgs&&... args) {
        return detail::HorizontalLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}
