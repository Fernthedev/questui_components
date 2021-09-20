#pragma once

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct ModifierContainer : Container<TArgs...> {
            static_assert(renderable<ModifierContainer<TArgs...>>);
            ModifierContainer(TArgs... args) : Container<TArgs...>(args...) {}
            auto render(RenderContext& ctx) {
                // TODO: Cache this properly
                auto obj = QuestUI::BeatSaberUI::CreateModifierContainer(&ctx.parentTransform)->get_transform();
                RenderContext ctx2(obj);
                Container<TArgs...>::render(ctx2);
                return obj;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto ModifierContainer(TArgs&&... args) {
        return detail::ModifierContainer<TArgs...>(std::forward<TArgs>(args)...);
    }
}
