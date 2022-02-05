#pragma once

#include <span>
#include <vector>

namespace QUC {
    template<class C>
    struct ROContainer {
        constexpr ROContainer() = default;
        constexpr ROContainer(C const& c) : container(c) {}
        constexpr operator std::span<typename C::value_type>() {return container;}
        constexpr operator std::span<typename C::value_type const>() const {return container;}
    private:
        C container;
    };
}