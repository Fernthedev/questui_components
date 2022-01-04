#pragma once

#include <concepts>
#include <tuple>

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

namespace QUC {
    struct RenderContext {
        /// @brief The parent transform to render on to.
        UnityEngine::Transform& parentTransform;
        // TODO: Add a way of performing tree diffs, context to context.
        // For now, if we ever need to trigger a rebuild we simply say that we need to on the component side.
        // Alternatively, any time we show a component, we call render, which will generate what we need and set everything accordingly.
        // For now, we will do this, though there are quite a few optimizations we may be able to make in the future.

        constexpr RenderContext(UnityEngine::Transform* ptr) : parentTransform(*ptr) {}
        constexpr RenderContext(UnityEngine::Transform& ref) : parentTransform(ref) {}
    };

    static void DestroyTree(RenderContext& ctx) {
        UnityEngine::Object::Destroy(ctx.parentTransform.get_gameObject());
    }

    // Allows both copies and references
    template<class T>
    /// @brief A concept for renderable components.
    /// @tparam T The type to check.
    concept renderable = requires (T t, RenderContext c) {
        t.render(c);
    };

    template<class T, class U>
    concept pointer_type_match = std::is_pointer_v<T> && std::is_convertible_v<T, U>;

    template<class T, class U>
    concept renderable_return = requires (T t, RenderContext c) {
        {t.render(c)} -> pointer_type_match<U>;
    };

    template<class T>
    /// @brief A concept for updatable components.
    /// @tparam T The type to check.
    concept updatable = requires (T t) {
        {t.update()};
    };

    template<class T>
    /// @brief A concept for cloneable components.
    /// @tparam T The type to check.
    concept cloneable = requires (T const t) {
        {t.clone()} -> std::same_as<T>;
    };

    namespace detail {
        template<size_t idx = 0, class... TArgs>
        requires ((renderable<TArgs> && ...))
        static constexpr void renderTuple(std::tuple<TArgs...>& args, RenderContext& ctx) {
            if constexpr (idx < sizeof...(TArgs)) {
                std::get<idx>(args).render(ctx);
                renderTuple<idx + 1>(args, ctx);
            }
        }

        template<size_t idx = 0, class... TArgs>
        requires ((cloneable<TArgs> && ...))
        std::tuple<TArgs...> cloneTuple(std::tuple<TArgs...> const& args) {
            if constexpr (idx < sizeof...(TArgs)) {
                auto clone = std::get<idx>(args).clone();
                auto newTuple = cloneTuple<idx + 1>(args);
                return std::make_tuple<TArgs...>(clone, newTuple);
            }
        }
    }
}