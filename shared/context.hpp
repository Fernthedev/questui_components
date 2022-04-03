#pragma once

#include "key.hpp"
#include "UnsafeAny.hpp"

#include <concepts>
#include <tuple>
#include <any>

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

namespace QUC {

    struct RenderContext;

    template<typename RenderContextT = RenderContext>
    struct RenderContextChildDataT {
        UnsafeAny childData;
        RenderContextT childContext;

        RenderContextChildDataT(RenderContextT childContext) : childContext(std::move(childContext)) {}

        constexpr void destroy() {
            childData = UnsafeAny();
            if (&childContext.parentTransform != nullptr && childContext.parentTransform.dyn_m_CachedPtr()) {
                UnityEngine::Object::Destroy(childContext.parentTransform.get_gameObject());
            }
        }

        template<typename T>
        [[nodiscard]] constexpr T& getData() {
            if (!childData.isType<T>()) {
                destroy();
            }

            if (!childData.has_value()) {
                return childData.make_any<T>();
            }

            return childData.get_any<T>();
        }

        [[nodiscard]] constexpr bool hasData() const {
            return childData.has_value();
        }

        template<typename T>
        [[nodiscard]] constexpr T const& getData() const {
            return childData.get_any<T>();
        }

        constexpr RenderContextT& getChildContext() {
            return childContext;
        }
    };

    using RenderContextChildData = RenderContextChildDataT<RenderContext>;

    struct RenderContext {
        using ChildContextKey = size_t; // 64 bit number
        using ChildData = Il2CppObject*;
        using ChildTransform = UnityEngine::Transform*;

        /// @brief The parent transform to render on to.
        UnityEngine::Transform& parentTransform;

        // For now, if we ever need to trigger a rebuild we simply say that we need to on the component side.
        // Alternatively, any time we show a component, we call render, which will generate what we need and set everything accordingly.
        // For now, we will do this, though there are quite a few optimizations we may be able to make in the future.

        RenderContext(UnityEngine::Transform* ptr) : parentTransform(*ptr) {}
        RenderContext(UnityEngine::Transform& ref) : parentTransform(ref) {}

        RenderContext(RenderContext&&) = default;

        RenderContext& operator=(RenderContext&& other) {
            new (this) RenderContext(std::move(other));
            return *this;
        }

        /**
         * Finds the child data if it exists through children
         * @param index
         * @return
         */
         template<bool recursive = true>
         std::optional<std::reference_wrapper<RenderContextChildData>> findChildData(ChildContextKey index) {
            auto it = dataContext.find(index);

            if (it != dataContext.end()) {
                return std::make_optional(std::ref(it->second));
            }

            if constexpr (recursive) {
                for (auto& [childKey, childContext]: dataContext) {
                    auto opt = childContext.childContext.findChildData<recursive>(index);

                    if (!opt) continue;

                    return opt;
                }
            }

            return std::nullopt;
        }

        template<bool recursive = false>
        bool hasChild(ChildContextKey index) {
            if (dataContext.contains(index)) return true;

            if constexpr (recursive) {
                for (auto const& [childKey, childContext] : dataContext) {
                    auto contains = childContext.childContext.template hasChild<recursive>(index);

                    if (!contains) continue;

                    return contains;
                }
            }

            return false;
        }

        template<bool recursive = true>
        [[nodiscard]] std::optional<std::reference_wrapper<const RenderContextChildData>> findChildData(ChildContextKey index) const {
            auto it = dataContext.find(index);

            if (it != dataContext.end()) {
                return std::make_optional(std::cref(it->second));
            }

            if constexpr (recursive) {
                for (auto& [childKey, childContext]: dataContext) {
                    auto opt = childContext.childContext.findChildData<recursive>(index);

                    if (!opt) continue;

                    return opt;
                }
            }

            return std::nullopt;
        }

        auto& getChildDataOrCreate(ChildContextKey index) {
            return dataContext[index];
        }

        // TODO: Uncomment
//        [[deprecated("Bad name")]]
        auto& getChildData(ChildContextKey index) {
            return getChildDataOrCreate(index);
        }

#pragma region child Context Clutter
        void destroyChildContext(ChildContextKey id) {
            auto contextIt = dataContext.find(id);
            if (contextIt == dataContext.end()) return;

            auto& context = contextIt->second;

            context.destroy();

            dataContext.erase(contextIt);
        }
#pragma endregion

        template<bool includeParent = false>
        void destroyTree() {
            // yeah yeah I know stupid
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-bool-conversion"
            if (!&parentTransform)
                return;
#pragma clang diagnostic pop

            if (parentTransform.dyn_m_CachedPtr()) {
                if constexpr (includeParent) {
                    UnityEngine::Object::Destroy(parentTransform.get_gameObject());
                } else {
                    int childCount = parentTransform.GetChildCount();
                    std::vector<UnityEngine::Transform*> transforms;
                    for (int i = 0; i < childCount; i++) {
                        auto transform = parentTransform.GetChild(i);
                        transforms.emplace_back(transform);
                    }
                    for (auto transform :transforms) {
                        UnityEngine::Object::Destroy(transform->get_gameObject());
                    }
                }
            }
            dataContext.clear();
        }

        template<bool destroyGO = true>
        void destroyChild(ChildContextKey key) {
            auto it = dataContext.find(key);
            if (it == dataContext.end()) return;

            auto& o = it->second;

            o.destroy();

            dataContext.erase(it);
        }


    private:
        // TODO: Figure out cleaning unusued keys
        std::unordered_map<ChildContextKey, RenderContextChildData> dataContext;
    };

    template<class T, class U>
    concept pointer_type_match = std::is_pointer_v<T> && std::is_convertible_v<T, U>;

    template<class T, class U>
    concept renderable_return = requires (T t, RenderContext& c, RenderContextChildData& data) {
        {t.render(c, data)} -> pointer_type_match<U>;
    };

    namespace detail {
        template<class T, class U>
        concept not_same_as = !std::is_same_v<T, U>;

        template<class T>
        concept renderable_tree_stub = requires(T const t) {
            {t.render()} -> not_same_as<void>;
        };
    }

    // Allows both copies and references
    template<class T>
    /// @brief A concept for renderable components.
    /// @tparam T The type to check.
    concept renderable_unity = renderable_return<T, UnityEngine::Transform*> && requires(T const t) {
        {t.key} -> keyed;
    };

    // Allows both copies and references
    /// @brief A concept for renderable components.
    /// @tparam T The type to check.
    template<class T>
    concept renderable_tree = requires (T const t) {
        {t.render()} -> detail::renderable_tree_stub;
    };

    template<class T>
    concept renderable = renderable_tree<T> || renderable_unity<T>;


    template<class T>
    /// @brief A concept for updatable components.
    /// @tparam T The type to check.
    concept updatable = requires (T t, RenderContext c) {
        t;
//        {t.update()};
    };

    template<class T>
    /// @brief A concept for cloneable components.
    /// @tparam T The type to check.
    concept cloneable = requires (T t, RenderContext c) {
        t;
//        {t.clone()} -> std::same_as<T>;
    };

    namespace detail {
        template<class T, size_t ix>
        requires (renderable<T>)
        static constexpr auto renderSingle(T& child, RenderContext& ctx) {
            if constexpr(renderable_tree<T>) {
                auto ret = child.render();

                // I think we should use the same ix and ctx? think? I've honestly no clue
                return renderSingle<decltype(ret), ix>(ret, ctx);
            }else if constexpr (renderable_unity<T>) {
                auto& childData = ctx.getChildData(ix);
                auto ret = child.render(ctx, childData);

                if (childData.childContext.parentTransform != ret) {
                    childData.destroy();
                    childData.childContext.parentTransform = {ret};
                }

                return ret;
            }
        }

        template<size_t idx = 0, class... TArgs>
        requires ((renderable<TArgs> && ...))
        static constexpr void renderTuple(std::tuple<TArgs...>& args, RenderContext& ctx) {
            if constexpr (idx < sizeof...(TArgs)) {
                auto& child = std::get<idx>(args);
                renderSingle<decltype(child), idx>(child, ctx); // render child
                renderTuple<idx + 1>(args, ctx);
            }
        }

        template<typename T>
        requires (renderable<T>)
        static constexpr void renderDynamicList(std::span<T> const args, RenderContext& ctx) {
            int i = 0;
            for (auto& child : args) {
                renderSingle<T ,i>(child, ctx); // render child
                i++;
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