#pragma once

#include <utility>

namespace QUC {
    struct Key {
        constexpr Key() : addr(&addr) {}

        bool operator==(const Key &rhs) const = default;

    private:
        void* addr;
        friend class std::hash<QUC::Key>;
        friend class std::hash<const QUC::Key>;
    };
}

namespace std {
    template<>
    struct hash<QUC::Key> {
        std::size_t operator()(const QUC::Key& obj) const noexcept {
            std::hash<void*> hash;
            return hash(obj.addr);
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
