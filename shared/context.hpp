#pragma once

#include "key.hpp"

#include <concepts>
#include <tuple>
#include <any>

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

namespace QUC {
    template <typename T>
    struct PtrWrapper {
        T ptr;
    };

    struct UnsafeAny {
        UnsafeAny() = default;
        UnsafeAny(UnsafeAny const&) = delete;

        // TODO: Copy constructor copies the data with a new ptr?

        template<typename T, typename... TArgs>
        T& make_any(TArgs&&... args) {
            if (dtor != nullptr) dtor(data);
            data = new T(std::forward<TArgs>(args)...);
            dtor = &destroy_data<T>;
            return *static_cast<T*>(data);
        }

        template<typename T>
        T& get_any() {
            return *static_cast<T*>(data);
        }

        bool has_value() {
            return data != nullptr;
        }

        ~UnsafeAny() {
            if (dtor != nullptr)
                dtor(data);
            free(data);
        }

    private:

        template<typename T>
        static void destroy_data(void* ptr) {
            delete reinterpret_cast<T*>(ptr);
        }

        void* data = nullptr;
        void(*dtor)(void*) = nullptr;
    };

    struct RenderContextChildData {
        UnsafeAny childData;

        template<typename T>
        T& getData() {
            if (!childData.has_value()) {
                childData.make_any<T>();
            }
            return childData.get_any<T>();
        }
    };

    struct RenderContext {
        using ChildContextKey = Key; // 64 bit number
        using ChildData = Il2CppObject*;

        /// @brief The parent transform to render on to.
        UnityEngine::Transform& parentTransform;

        // For now, if we ever need to trigger a rebuild we simply say that we need to on the component side.
        // Alternatively, any time we show a component, we call render, which will generate what we need and set everything accordingly.
        // For now, we will do this, though there are quite a few optimizations we may be able to make in the future.

        RenderContext(UnityEngine::Transform* ptr) : parentTransform(*ptr) {}
        RenderContext(UnityEngine::Transform& ref) : parentTransform(ref) {}


        auto& getChildData(ChildContextKey index) {
            return dataContext[index];
        }

#pragma region child Context Clutter
        // TODO: Figure this out better
        // maybe not needed
        template<typename T, typename F = std::function<UnityEngine::Transform*()>>
        RenderContext& getChildContext(ChildContextKey id, F transform) {
            auto it = childrenContexts.find(id);

            std::hash<void*> hash;
            auto klassHash = hash(classof(T*));

            if (it == childrenContexts.end()) {
                return childrenContexts.try_emplace(id, klassHash, transform()).first->second.second;
            } else {
                auto& [klassHashObject, data] = it->second;


                if (klassHash == klassHashObject) {
                    return data;
                }

                if (data.parentTransform.m_CachedPtr) {
                    // Destroy old context tree
                    UnityEngine::Object::Destroy(data.parentTransform.get_gameObject());
                }

                return childrenContexts.try_emplace(id, klassHash, transform()).first->second.second;
            }
        }

        void destroyChildContext(ChildContextKey id) {
            auto contextIt = childrenContexts.find(id);
            if (contextIt != childrenContexts.end()) {
                auto& context = contextIt->second;
                auto const& data = context.second;
                if (data.parentTransform.m_CachedPtr) {
                    // Destroy old context tree
                    UnityEngine::Object::Destroy(data.parentTransform.get_gameObject());
                }

                childrenContexts.erase(contextIt);
            }
        }

#pragma endregion

        template<bool includeParent = false>
        void destroyTree() {
            if (parentTransform.m_CachedPtr) {
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
        }


    private:
//        template<typename InnerData>
//        struct RenderContextData {
//            // https://stackoverflow.com/a/994368/9816000
//            const size_t klassHash;
//            InnerData data;
//
//            template<typename T>
//            requires (std::is_convertible_v<T, Il2CppObject*>)
//            RenderContextData(InnerData const& data) : data(data) {
//                static auto klass = klassHash = std::hash<T>()(classof(T));
//            }
//        };


        // TODO: Figure out cleaning unusued keys
        std::unordered_map<ChildContextKey, RenderContextChildData> dataContext;
        std::unordered_map<ChildContextKey, std::pair<size_t, RenderContext>> childrenContexts;
    };

    template<typename T>
    concept keyed = std::is_convertible_v<T, const QUC::Key>;

    // Allows both copies and references
    template<class T>
    /// @brief A concept for renderable components.
    /// @tparam T The type to check.
    concept renderable = requires (T t, RenderContext c, RenderContextChildData& data) {
        t.render(c, data);
    } && requires(T const t) {
        {t.key} -> keyed;
    };

    template<class T, class U>
    concept pointer_type_match = std::is_pointer_v<T> && std::is_convertible_v<T, U>;

    template<class T, class U>
    concept renderable_return = requires (T t, RenderContext c, RenderContextChildData& data) {
        {t.render(c, data)} -> pointer_type_match<U>;
    };

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
        template<class T>
        requires (renderable<T>)
        static constexpr auto renderSingle(T& child, RenderContext& ctx) {
            auto& childData = ctx.getChildData(child.key);
            return child.render(ctx, childData);
        }

        template<size_t idx = 0, class... TArgs>
        requires ((renderable<TArgs> && ...))
        static constexpr void renderTuple(std::tuple<TArgs...>& args, RenderContext& ctx) {
            if constexpr (idx < sizeof...(TArgs)) {
                auto& child = std::get<idx>(args);
                renderSingle(child, ctx); // render child
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