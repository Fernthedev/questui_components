#pragma once

#include <malloc.h>
#include <utility>

namespace QUC {
    struct UnsafeAny {
        UnsafeAny() = default;
        UnsafeAny(UnsafeAny const&) = delete;

        template<typename T, typename... TArgs>
        T& make_any(TArgs&&... args) {
            if (dtor != nullptr) {
                dtor(data);
            }
            free(data);
            data = new T(std::forward<TArgs>(args)...);
            dtor = &destroy_data<T>;
            return get_any<T>();
        }

        template<typename T>
        T& get_any() {
            return *static_cast<T*>(data);
        }

        bool has_value() {
            return data != nullptr;
        }

        ~UnsafeAny() {
            if (dtor != nullptr)
                dtor(data);
            free(data);
        }

    private:

        template<typename T>
        static void destroy_data(void* ptr) {
            delete reinterpret_cast<T*>(ptr);
        }

        void* data = nullptr;
        void(*dtor)(void*) = nullptr;
    };
}