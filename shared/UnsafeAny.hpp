#pragma once

#include <malloc.h>
#include <utility>

#include <system_error>
#include <exception>

namespace QUC {
    struct UnsafeAny {
        constexpr UnsafeAny() = default;
        constexpr UnsafeAny(UnsafeAny const&) = delete;

        template<typename T, typename... TArgs>
        [[nodiscard]] constexpr T& make_any(TArgs&&... args) {
            if (dtor != nullptr) {
                dtor(data);
            }
            free(data);
            data = new T(std::forward<TArgs>(args)...);
            dtor = &destroy_data<T>;
            ptrSize = sizeof(T);
            return get_any<T>();
        }

        template<typename T>
        constexpr void validatePtrSize() const {
            if (ptrSize != sizeof(T))
                throw std::runtime_error("Ptr size does not match T size!");
        }

        template<typename T>
        [[nodiscard]] constexpr T& get_any() {
            validatePtrSize<T>();

            return *static_cast<T*>(data);
        }

        template<typename T>
        [[nodiscard]] constexpr T const& get_any() const {
            validatePtrSize<T>();

            return *static_cast<T const*>(data);
        }

        [[nodiscard]] constexpr bool has_value() const {
            return data != nullptr;
        }

        constexpr ~UnsafeAny() {
            if (dtor != nullptr)
                dtor(data);
            free(data);
        }

    private:

        template<typename T>
        constexpr static void destroy_data(void* ptr) {
            delete reinterpret_cast<T*>(ptr);
        }

        size_t ptrSize = -1;
        void* data = nullptr;
        void(*dtor)(void*) = nullptr;
    };
}