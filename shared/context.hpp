#pragma once

#include <concepts>
#include <tuple>

namespace UnityEngine {
    class Transform;
}

namespace QUC {
    struct RenderContext {
        UnityEngine::Transform* parentTransform;
        // TODO: Add a way of performing tree diffs, context to context.
        // For now, if we ever need to trigger a rebuild we simply say that we need to on the component side.
        // Alternatively, any time we show a component, we call render, which will generate what we need and set everything accordingly.
        // For now, we will do this, though there are quite a few optimizations we may be able to make in the future.
    };

    // Allows both copies and references
    template<class T>
    /// @brief A concept for renderable components.
    /// @tparam T The type to check.
    concept renderable = requires (T t, RenderContext c) {
        t.render(c);
    };

    template<class T>
    /// @brief A concept for updatable components.
    /// @tparam T The type to check.
    concept updatable = requires (T t) {
        {t.has_change()} -> std::same_as<bool>;
    };

    namespace detail {
        template<size_t idx = 0, class... TArgs>
        requires ((renderable<TArgs> && ...))
        void renderTuple(std::tuple<TArgs...>& args, RenderContext& ctx) {
            if constexpr (idx < sizeof...(TArgs)) {
                std::get<idx>(args).render(ctx);
                renderTuple<idx + 1>(args, ctx);
            }
        }
    }
}