#pragma once

#include <concepts>
#include <tuple>
#include <string_view>
#include <string>
#include <type_traits>
#include <optional>

namespace QUC {

    template<class T>
    struct HeldData;

    template<class T>
    /// @brief A concept for updatable components.
    /// @tparam T The type to check.
    concept HeldDataCheck = requires (T const t) {
        {t.isModified()} noexcept -> std::same_as<bool>;
        {t.getData()} noexcept;// -> std::convertible_to<T>;
        {*t} noexcept; //-> std::same_as<T const&>;
        {t.operator ->} noexcept; // -> std::convertible_to<T const&>;
        {(bool) t};

        std::is_default_constructible_v<T>;

        // TODO: How to static assert more constructors, constexpr and const
    } && requires(T t) {
        {t.clear()} noexcept;
    };

    template<class T>
    struct HeldData {
        HeldData() = default;

        constexpr HeldData(T const&) : data(data) {}

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr explicit(false) HeldData(const HeldData<U>& other) : modified(other.modified), data(other.data) {}

        template<class Q>
        requires (std::is_convertible_v<Q, T>)
        constexpr explicit(false) HeldData(Q&& arg) : data(arg) {}

        constexpr explicit(false) operator T () const noexcept {return data;}
        constexpr explicit(false) operator T const&() const noexcept {return data;}
        explicit(false) constexpr operator bool() const noexcept {return modified;}

        [[nodiscard]] constexpr bool isModified() const noexcept {
            return modified;
        }

        [[nodiscard]] constexpr T const& getData() const noexcept {
            return data;
        }

        constexpr void clear() noexcept {
            modified = false;
        }
        constexpr HeldData<T>& operator=(const T& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
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
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<T>& operator=(const U& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        constexpr T const& operator ->() const noexcept {
            return data;
        }

        constexpr T const& operator *() const noexcept {
            return data;
        }

    private:
        bool modified = false;
        T data;
    };
    static_assert(HeldDataCheck<HeldData<int>>);

    template<class T>
    struct HeldData<std::optional<T>> {
        HeldData() = default;

        constexpr HeldData(T const& data) : data(data) {}
        constexpr HeldData(std::optional<T> const& data) : data(data) {}

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr explicit(false) HeldData(const std::optional<U>& other) : data(data) {}


        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr explicit(false) HeldData(const HeldData<U>& other) : modified(other.modified), data(other.data) {}

        template<class Q>
        requires (std::is_convertible_v<Q, T>)
        constexpr explicit(false) HeldData(Q&& arg) : data(arg) {}

        constexpr explicit(false) operator std::optional<T> () const noexcept {return data;}
        constexpr explicit(false) operator std::optional<T> const&() const noexcept {return data;}
        explicit(false) constexpr operator bool() const noexcept {return modified;}

        [[nodiscard]] constexpr bool isModified() const noexcept {
            return modified;
        }

        [[nodiscard]] constexpr std::optional<T> const& getData() const noexcept {
            return data;
        }

        constexpr void clear() noexcept {
            modified = false;
        }
        constexpr HeldData<std::optional<T>>& operator=(const T& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const HeldData<T>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const std::optional<T>& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const HeldData<std::optional<T>>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const HeldData<U>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const std::optional<U>& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const U& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const HeldData<std::optional<U>>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }


        constexpr std::optional<T> const& operator ->() const noexcept {
            return data;
        }

        constexpr std::optional<T> const& operator *() const noexcept  {
            return data;
        }

    private:
        bool modified = false;
        std::optional<T> data;
    };
    static_assert(HeldDataCheck<HeldData<std::optional<int>>>);
    static_assert(HeldDataCheck<HeldData<std::optional<bool>>>);

    template<>
    struct HeldData<bool> {
        HeldData() = default;

        constexpr HeldData(bool data) : data(data) {};

        constexpr explicit(false) operator bool () const noexcept {return modified;}

        [[nodiscard]] constexpr bool isModified() const noexcept {
            return modified;
        }

        [[nodiscard]] constexpr bool const& getData() const noexcept {
            return data;
        }

        constexpr void clear() noexcept {
            modified = false;
        }

        constexpr HeldData<bool>& operator=(const HeldData<bool>& other) {
            if (data != other.data) {
                modified = true;
                data = other.data;
            }
            return *this;
        }

        constexpr HeldData<bool>& operator=(bool other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        constexpr bool const& operator *() const noexcept {
            return data;
        }

        constexpr bool const& operator ->() const noexcept {
            return data;
        }

    private:
        bool modified = false;
        bool data;
    };
    static_assert(HeldDataCheck<HeldData<bool>>);

    template<>
    struct HeldData<std::string> {
        HeldData() = default;

        explicit(false) HeldData(std::string_view d) : data(d) {}

        template<size_t sz>
        constexpr explicit(false) HeldData(const char (&str)[sz]) : data(str) {}

        template<class U>
        requires (std::is_convertible_v<U, std::string>)
        constexpr explicit(false) HeldData(const HeldData<U>& other) : modified(other.modified), data(other.data) {}

        explicit(false) operator std::string const& () const {return data;}
        explicit(false) constexpr operator bool() const noexcept {return modified;}

        [[nodiscard]] constexpr bool isModified() const noexcept {
            return modified;
        }

        [[nodiscard]] constexpr std::string const& getData() const noexcept {
            return data;
        }

        constexpr void clear() noexcept {
            modified = false;
        }

        constexpr HeldData<std::string>& operator=(std::string_view other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
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

        template<class U>
        requires (std::is_convertible_v<U, std::string>)
        constexpr HeldData<std::string>& operator=(const U& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
            return *this;
        }

        constexpr std::string const& operator ->() const noexcept {
            return data;
        }

        constexpr std::string const& operator *() const noexcept {
            return data;
        }

    private:
        bool modified = false;
        std::string data;
    };
    static_assert(HeldDataCheck<HeldData<std::string>>);
}