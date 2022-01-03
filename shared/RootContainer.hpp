#pragma once

#include "context.hpp"
#include <tuple>

namespace QUC {
    namespace detail {
        // Represents a generic container
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
        struct Container {
            std::tuple<TArgs...> children;
            Container(TArgs... args) : children(args...) {}
            Container(std::tuple<TArgs...> args) : children(args) {}

            void render(RenderContext& ctx) {
                QUC::detail::renderTuple(children, ctx);
            }

            [[nodiscard]] std::tuple<TArgs...> clone() const {
                return QUC::detail::cloneTuple(children);
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto Container(TArgs&&... args) {
        return detail::Container<TArgs...>(std::forward<TArgs>(args)...);
    }
}
