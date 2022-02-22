#pragma once

#include "shared/RootContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "CommonLayout.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct VerticalLayoutGroup : public Container<TArgs...>, public ModifyLayout {
            VerticalLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}

            constexpr UnityEngine::RectTransform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& viewLayout = data.getData<UnityEngine::UI::VerticalLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!viewLayout) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    viewLayout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(&parent);
                }

                RenderContext& childrenCtx = data.getChildContext([viewLayout] {
                    return viewLayout->get_rectTransform();
                });
                ModifyLayout::assignLayout(ctx, viewLayout);
                detail::Container<TArgs...>::render(childrenCtx, data);
                return static_cast<UnityEngine::RectTransform *>(&childrenCtx.parentTransform);
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto VerticalLayoutGroup(TArgs&&... args) {
        return detail::VerticalLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}