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
                if (nameStr) {
                    auto found = parent.FindChild(nameStr);
                    if (found) {
                        // It's actually EASIER for us to destroy and remake the entire tree instead of changing some elements.
                        // TODO: Eventually this may change
                        UnityEngine::Object::Destroy(found);
                    }
                }
                auto container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(&parent);
                if (!nameStr) {
                    using namespace il2cpp_utils;
                    // TODO: This might not be the correct quantity of parenting
                    nameStr = newcsstr<CreationType::Manual>(csstrtostr(container->get_transform()->GetParent()->get_gameObject()->get_name()));
                }

                RenderContext childrenCtx(container->get_transform());
                Container<TArgs...>::render(childrenCtx);
                return container->get_transform();
            }

            private:
            static inline Il2CppString* nameStr = nullptr;
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto ScrollableContainer(TArgs&&... args) {
        return detail::ScrollableContainer<TArgs...>(std::forward<TArgs>(args)...);
    }
}