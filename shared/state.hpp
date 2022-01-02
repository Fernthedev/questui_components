#pragma once

#include <concepts>
#include <tuple>
#include <string_view>
#include <string>

namespace QUC {

    template<class T>
    struct HeldData {
        bool modified = false;
        T data;
        HeldData() = default;
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr explicit(false) HeldData(const HeldData<U>& other) : modified(other.modified), data(other.data) {}
        template<class Q>
        requires (std::is_convertible_v<Q, T>)
        constexpr explicit(false) HeldData(Q&& arg) : data(arg) {}

        constexpr explicit(false) operator T const&() const {return data;}
        constexpr operator bool() const {return modified;}
        constexpr void clear() {
            modified = false;
        }
        constexpr HeldData<T>& operator=(const HeldData<T>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<T>& operator=(const HeldData<U>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }
    };

    template<>
    struct HeldData<std::string> {
        bool modified = false;
        std::string data;
        HeldData() = default;

        explicit(false) HeldData(std::string_view d) : data(d) {}

        template<size_t sz>
        constexpr explicit(false) HeldData(const char (&str)[sz]) : data(str) {}

        template<class U>
        requires (std::is_convertible_v<U, std::string>)
        constexpr explicit(false) HeldData(const HeldData<U>& other) : modified(other.modified), data(other.data) {}

        explicit(false) operator std::string const& () const {return data;}
        explicit(false) constexpr operator bool() const {return modified;}
        constexpr void clear() {
            modified = false;
        }
        constexpr HeldData<std::string>& operator=(const HeldData<std::string>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, std::string>)
        constexpr HeldData<std::string>& operator=(const HeldData<U>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }
    };
}