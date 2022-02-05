#pragma once

#include "context.hpp"
#include "components/Mock.hpp"
#include "ROContainer.hpp"
#include <tuple>
#include <span>

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

        static_assert(renderable<Container<MockComp>>);

        template <typename Component, typename InnerList = std::vector<Component>>
        requires (renderable<Component>)
        struct VariableContainer
        {
            ROContainer<InnerList> children; // TODO: Don't allow this to be reassigned
            const Key key;

            VariableContainer(InnerList const& children) : children(children) {}

            VariableContainer(std::initializer_list<Component> const children) : children(children) {}

            VariableContainer(std::span<Component> const children) : children(children) {}

            void render(RenderContext& ctx, RenderContextChildData& data) {
                QUC::detail::renderDynamicList<Component>(children, ctx);
            }
        };

        static_assert(renderable<VariableContainer<MockComp>>);
        static_assert(renderable<VariableContainer<MockComp, std::vector<MockComp>>>);
        static_assert(renderable<VariableContainer<MockComp, std::span<MockComp>>>);
    }
    template<class... TArgs>
    requires ((renderable<TArgs> && ...))
    auto Container(TArgs&&... args) {
        return detail::Container<TArgs...>(std::forward<TArgs>(args)...);
    }
}
