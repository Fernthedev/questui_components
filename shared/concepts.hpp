#pragma once

#include <concepts>

namespace QUC {
    template<typename T, typename U>
    concept IsQUCConvertible = std::is_convertible_v<T, U>;
}