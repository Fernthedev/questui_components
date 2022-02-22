#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "CommonLayout.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct HorizontalLayoutGroup : public Container<TArgs...>, public ModifyLayout {
            constexpr HorizontalLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}

            constexpr UnityEngine::RectTransform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& horizontalLayout = data.getData<UnityEngine::UI::HorizontalLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!horizontalLayout) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    horizontalLayout = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(&parent);
                }

                RenderContext& childrenCtx = data.getChildContext([horizontalLayout]() {
                    return horizontalLayout->get_rectTransform();
                });
                ModifyLayout::assignLayout(ctx, horizontalLayout);
                detail::Container<TArgs...>::render(childrenCtx, data);
                return static_cast<UnityEngine::RectTransform *>(&childrenCtx.parentTransform);
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto HorizontalLayoutGroup(TArgs&&... args) {
        return detail::HorizontalLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}
