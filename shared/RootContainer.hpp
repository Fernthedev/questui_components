#pragma once

#include "context.hpp"
#include <tuple>

namespace QUC {
    namespace detail {
        // Represents a generic container
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct Container {
            static_assert(QUC::renderable<Container<TArgs...>>);
            std::tuple<TArgs...> children;
            constexpr Container(TArgs... args) : children(args...) {}

            void render(RenderContext& ctx) {
                QUC::detail::renderTuple(children, ctx);
            }
        };
    }
    template<class... TArgs>
    auto Container(TArgs&&... args) {
        return detail::Container<TArgs...>(std::forward<TArgs>(args)...);
    }
}
