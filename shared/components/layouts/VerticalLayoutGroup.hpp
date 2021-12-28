#pragma once

#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct VerticalLayoutGroup : Container<TArgs...> {
            VerticalLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}
            auto render(RenderContext& ctx) {
                // TODO: Cache this properly
                auto obj = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(&ctx.parentTransform)->get_transform();
                RenderContext ctx2(obj);
                Container<TArgs...>::render(ctx2);
                return obj;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto VerticalLayoutGroup(TArgs&&... args) {
        return detail::VerticalLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}