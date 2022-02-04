#pragma once

#include "shared/context.hpp"

// Components used for mock testing and static assert

namespace QUC {
    struct MockComp {
        const Key key;

        MockComp() = default;

        constexpr void render(RenderContext& ctx, RenderContextChildData& data) {}
    };

    struct MockUnityComp {
        const Key key;

        MockUnityComp() = default;

        constexpr UnityEngine::Transform* render(RenderContext& ctx, RenderContextChildData& data) {
            return nullptr;
        }
    };

    static_assert(renderable<MockComp>);
    static_assert(renderable_return<MockUnityComp, UnityEngine::Transform*>);
}
