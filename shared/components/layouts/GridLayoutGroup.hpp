#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"
#include "UnityEngine/UI/GridLayoutGroup.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "CommonLayout.hpp"

namespace QUC {
    namespace detail {
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct GridLayoutGroup : public Container<TArgs...>, public ModifyLayout {
            static_assert(renderable<GridLayoutGroup<TArgs...>>);
            constexpr GridLayoutGroup(TArgs... args) : Container<TArgs...>(args...) {}

            constexpr UnityEngine::RectTransform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& gridLayoutGroup = data.getData<UnityEngine::UI::GridLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!gridLayoutGroup) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    gridLayoutGroup = QuestUI::BeatSaberUI::CreateGridLayoutGroup(&parent);
                }

                RenderContext& childrenCtx = ctx.getChildData([gridLayoutGroup]() {
                    return gridLayoutGroup->get_rectTransform();
                });
                ModifyLayout::assignLayout(ctx, gridLayoutGroup);
                detail::Container<TArgs...>::render(childrenCtx, data);
                return static_cast<UnityEngine::RectTransform *>(&childrenCtx.parentTransform);
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto GridLayoutGroup(TArgs&&... args) {
        return detail::GridLayoutGroup<TArgs...>(std::forward<TArgs>(args)...);
    }
}
