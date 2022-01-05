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

            const Key key;

            UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
                auto& modifierLayout = data.getData<UnityEngine::UI::VerticalLayoutGroup*>();
                auto &parent = ctx.parentTransform;
                if (!modifierLayout) {
                    // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                    modifierLayout = QuestUI::BeatSaberUI::CreateModifierContainer(&parent);
                }

                RenderContext& childrenCtx = ctx.getChildContext<UnityEngine::UI::VerticalLayoutGroup>(key, [modifierLayout]() {
                    return modifierLayout->get_transform();
                });
                detail::Container<TArgs...>::render(childrenCtx, data);
                return &childrenCtx.parentTransform;
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto ModifierContainer(TArgs&&... args) {
        return detail::ModifierContainer<TArgs...>(std::forward<TArgs>(args)...);
    }
}
