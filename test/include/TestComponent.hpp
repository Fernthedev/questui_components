#pragma once

#include "shared/components/Text.hpp"
#include "shared/RootContainer.hpp"

#include <utility>
#include <vector>
#include <string>


namespace UnityEngine {
    class Transform;
}

namespace QUC {
    static auto TestComponent(std::string_view prefix) {
        return Container(
                Text(std::string(prefix) + "Group 1"),
                Text(std::string(prefix) + "Group 2")
        );
    }

    // Renders a list of text progressively
    struct MoreComplexType {
        // Identifies this component in the tree
        const Key key;

        std::string prefix;
        std::array<Text, 4> texts = {Text(prefix + "text 1"), Text(prefix + "text 2"), Text(prefix + "text 3"), Text(prefix + "text 4")};

        MoreComplexType(std::string_view prefix) : prefix(prefix) {}

        void render(RenderContext& ctx, RenderContextChildData& data) {
            // get state in the tree
            auto& timesRendered = data.getData<int>();
            if (timesRendered >= 0 && timesRendered < 4) {
                detail::renderSingle(texts[timesRendered], ctx);
                timesRendered++;
            }
        }
    };
    static_assert(renderable<MoreComplexType>);
}