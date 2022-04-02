#pragma once

#include <utility>
#include <chrono>

namespace QUC {
    struct Key {
        Key() : seed(std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::high_resolution_clock::now().time_since_epoch()).count())
        {}

        constexpr Key(Key const&) = default;

        bool operator==(const Key &rhs) const = default;

    protected:
        constexpr Key(uint64_t seed) : seed(seed) {}

    private:
        uint64_t seed;
        friend class std::hash<QUC::Key>;
        friend class std::hash<const QUC::Key>;
    };

    template<typename T>
    concept keyed = std::is_convertible_v<T, const QUC::Key>;
}

namespace std {
    template<>
    struct hash<QUC::Key> {
        [[deprecated("warn")]]
        constexpr std::size_t operator()(const QUC::Key& obj) const noexcept {
            std::hash<uint64_t> hash;
            return hash(obj.seed);
        }
    };

//    template<>
//    struct hash<const QUC::Key> {
//        std::size_t operator()(const QUC::Key& obj) const noexcept {
//            std::hash<void*> hash;
//            return hash(obj.addr);
//        }
//    };
}
