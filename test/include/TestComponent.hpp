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

    // TODO: More complex custom component
}