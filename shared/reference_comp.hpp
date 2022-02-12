#pragma once

#include "context.hpp"
#include "components/Mock.hpp"
#include "ROContainer.hpp"
#include <tuple>
#include <span>

namespace QUC {
    template<typename T>
    struct RefComp {
        T& child;
        const Key key;

        constexpr RefComp(T& a) : child(a) {}

//        template<typename... TArgs>
//        constexpr RefComp(TArgs&&... args) : child(std::forward<TArgs>(args)...) {}

        constexpr void render(RenderContext& ctx, RenderContextChildData& data) {
            QUC::detail::renderSingle(child, ctx);
        }

    };

    namespace detail {
//        template<typename T, typename... TArgs>
//        constexpr RefComp<T> refComp(TArgs&&... args) {
//            return RefComp<T>(std::forward<TArgs>(args)...);
//        }

        template<typename T>
        constexpr RefComp<T> refComp(T& args) {
            return RefComp<T>(args);
        }
    }
}