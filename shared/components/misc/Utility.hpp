#pragma once

#include "shared/context.hpp"
#include "shared/RootContainer.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"


namespace QUC {
    template<class T>
    requires (QUC::renderable_return<T, UnityEngine::Transform*>)
    struct OnRenderCallback {
        std::remove_reference_t<T> child;
        const QUC::Key key;

        using Callback = std::function<void(T& child, QUC::RenderContext& ctx, QUC::RenderContextChildData& data)>;

        Callback renderCallback;

        template<typename F>
        OnRenderCallback(T child_, F&& c) : child(child_), renderCallback(c), key(child_.key) {} // use child's key since we just wrap

        UnityEngine::Transform* render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) {
            auto res = QUC::detail::renderSingle(child, ctx);

            if (renderCallback) {
                renderCallback(child, ctx, data);
            }

            return res;
        }
    };


    // Component to return the transform of the parent
    template<class... TArgs>
    requires ((QUC::renderable<TArgs> && ...))
    struct WrapParent : public QUC::detail::Container<TArgs...> {
        constexpr WrapParent(TArgs... args) : QUC::detail::Container<TArgs...>(args...) {}
        constexpr WrapParent(std::tuple<TArgs...> args) : QUC::detail::Container<TArgs...>(args) {}

        constexpr auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) {
            QUC::detail::Container<TArgs...>::render(ctx, data);
            return &ctx.parentTransform;
        }
    };

}
