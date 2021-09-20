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
        struct ScrollableContainer {
            static_assert(QUC::renderable<Container<TArgs...>>);
            std::tuple<TArgs...> children;
            ScrollableContainer(TArgs... args) : children(args...) {}

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
                    nameStr = newcsstr<CreationType::Manual>(csstrtostr(container->get_transform()->GetParent()->get_gameObject()->get_name()));
                }

                RenderContext childrenCtx(container->get_transform());
                QUC::detail::renderTuple(children, childrenCtx);
                return container->get_transform();
            }

            private:
            static inline Il2CppString* nameStr = nullptr;
        };
    }
}