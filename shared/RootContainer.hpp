#pragma once

#include "context.hpp"
#include <tuple>

namespace QUC {
    namespace detail {
        // Represents a generic container
        template<class... TArgs>
        requires ((renderable<TArgs> && ...))
//        requires ((!std::is_reference_v<TArgs> && ...) && ((renderable<TArgs> && ...)))
        struct Container {
            std::tuple<TArgs...> children;
            const Key key;
            Container(TArgs... args) : children(args...) {}
            Container(std::tuple<TArgs...> args) : children(args) {}


            [[nodiscard]] std::tuple<TArgs...> clone() const {
                return QUC::detail::cloneTuple(children);
            }


            void render(RenderContext& ctx, RenderContextChildData& data) {
                QUC::detail::renderTuple(children, ctx);
            }
        };

        template<typename Component, typename InnerList = std::vector<Component>>
        struct VariableContainer {
            const InnerList children;

            VariableContainer(InnerList const& children) : children(children) {}

            VariableContainer(std::initializer_list<Component> const children) : children(children) {}

            void render(RenderContext& ctx, RenderContextChildData& data) {
                QUC::detail::renderTuple(children, ctx);
            }
        };
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto Container(TArgs&&... args) {
        return detail::Container<TArgs...>(std::forward<TArgs>(args)...);
    }
}
