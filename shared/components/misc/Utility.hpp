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

    template<class... TArgs>
    requires ((QUC::renderable<TArgs> && ...))
    struct DebugLogTree : public QUC::detail::Container<TArgs...> {

        Logger& logger;

        constexpr DebugLogTree(Logger& logger, TArgs... args) : logger(logger), QUC::detail::Container<TArgs...>(args...) {}
        constexpr DebugLogTree(Logger& logger, std::tuple<TArgs...> args) : logger(logger), QUC::detail::Container<TArgs...>(args) {}

        void log(UnityEngine::Transform* ctx, int subIndex = 1) {
            std::string indent(subIndex * 4, ' ');
            logger.debug("%s %s", indent.c_str(), static_cast<std::string>(ctx->get_name()).c_str());

            int childCount = ctx->GetChildCount();
            for (int i = 0; i < childCount; ++i) {
                auto child = ctx->GetChild(i);

                log(child, subIndex);
            }
        }

//  template<size_t idx = 0, class... TTArgs>
//        requires ((renderable<TTArgs> && ...))
//        static void renderAndLogTuple(std::tuple<TTArgs...>& args, RenderContext& ctx, Logger& logger, int subIndex = 1) {
//            if constexpr (idx < sizeof...(TArgs)) {
//                auto& child = std::get<idx>(args);
//
//                auto ret = renderSingle(child, ctx); // render child
//
//                if constexpr (requires(decltype(ret) ret) {ret->get_name();}) {
//                    if (ret) {
//                        std::string indent(' ', subIndex * 4);
//
//                        logger.debug("%s %s", indent.c_str(), static_cast<std::string>(ret->get_name()).c_str());
//                    }
//                } else {
//                    std::optional<std::reference_wrapper<RenderContextChildData>> const& childData = ctx.findChildData(child.key);
//
//                    if (childData) {
//                        childData->get().
//                    }
//                }
//
//                renderTuple<idx + 1>(args, ctx);
//            }
//        }

        constexpr auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) {
            QUC::detail::Container<TArgs...>::render(ctx, data);

            log(ctx);

            return &ctx.parentTransform;
        }
    };

    template<typename T>
    struct FunctionalComponent {
        const QUC::Key key;
        std::function<T(QUC::RenderContext& ctx, QUC::RenderContextChildData& data )> buildComp;

        template<typename F>
        FunctionalComponent(F&& func) : buildComp(func) {}

        constexpr auto render(QUC::RenderContext& ctx, QUC::RenderContextChildData& data) const {
            return buildComp(ctx, data);
        }

    };

    // Template deduction guide that makes life great again
    // it
    template<typename F>
    FunctionalComponent(F&& f) -> FunctionalComponent<typename decltype(std::function(f))::result_type>;

//    // function_ptr_t courtesy of DaNike
//    template<typename TRet, typename ...TArgs>
//// A generic function pointer, which can be called with and set to a `getRealOffset` call
//    using function_ptr_t = TRet(*)(TArgs...);
//
//    template<typename C, typename TRet, typename ...TArgs>
//    using function_ptr_t_member = TRet(C::*)(TArgs...);
//
//    template<typename T>
//    FunctionalComponent(std::function<T(QUC::RenderContext& ctx, QUC::RenderContextChildData& data)> func) -> FunctionalComponent<typename decltype(func)::result_type>;
//

//
//    template<typename T>
//    FunctionalComponent(function_ptr_t<T, QUC::RenderContext&, QUC::RenderContextChildData&> func) -> FunctionalComponent<T>;
//
//
//    template<typename C, typename T>
//    FunctionalComponent(function_ptr_t_member<C, T, QUC::RenderContext&, QUC::RenderContextChildData&> func) -> FunctionalComponent<T>;
}
