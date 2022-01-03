#pragma once

#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/TextPageScrollView.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct ScrollableContainer : Container<TArgs...> {
            ScrollableContainer(TArgs... args) : Container<TArgs...>(args...) {}

            auto render(RenderContext& ctx) {
                auto& parent = ctx.parentTransform;
                if (!scrollContainer) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    scrollContainer = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(&parent);
                }

                RenderContext childrenCtx(scrollContainer->get_transform());
                Container<TArgs...>::render(childrenCtx);
                return childrenCtx.parentTransform;
            }

            private:
            WeakPtrGO<UnityEngine::GameObject> scrollContainer;
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto ScrollableContainer(TArgs&&... args) {
        return detail::ScrollableContainer<TArgs...>(std::forward<TArgs>(args)...);
    }
}