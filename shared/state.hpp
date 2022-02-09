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

        constexpr HeldData(T const& data) : data(data) {}

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

        template<typename A>
        constexpr void emplace(A const& other) {
            if (data != other) {
                modified = true;
                data = other;
            }
        }

        template<typename A>
        constexpr void emplace(HeldData<A> const& other) {
            if (data != other.getData()) {
                modified = true;
                data = other.getData();
            }
        }

        constexpr HeldData<T>& operator=(const T& other) {
            emplace<T>(other);
            return *this;
        }

        constexpr HeldData<T>& operator=(const HeldData<T>& other) {
            emplace(other);
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<T>& operator=(const HeldData<U>& other) {
            emplace(other);
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<T>& operator=(const U& other) {
            emplace<U>(other);
            return *this;
        }

        constexpr T const& operator ->() const noexcept {
            return data;
        }

        constexpr T const& operator *() const noexcept {
            return data;
        }

    protected:
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

        template<typename A>
        constexpr void emplace(const A other) {
            if (data != other) {
                modified = true;
                data = other;
            }
        }

        template<typename A>
        constexpr void emplace(HeldData<A> const& other) {
            if (data != other.getData()) {
                modified = true;
                data = other.getData();
            }
        }

        constexpr HeldData<std::optional<T>>& operator=(const T& other) {
            emplace(other);
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const HeldData<T>& other) {
            emplace(other);
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const std::optional<T>& other) {
            emplace(other);
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const HeldData<std::optional<T>>& other) {
            emplace(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const HeldData<U>& other) {
            emplace(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const std::optional<U>& other) {
            emplace(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const U& other) {
            emplace<U>(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const HeldData<std::optional<U>>& other) {
            emplace(other);
            return *this;
        }


        constexpr std::optional<T> const& operator ->() const noexcept {
            return data;
        }

        constexpr std::optional<T> const& operator *() const noexcept  {
            return data;
        }

    protected:
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

        template<typename A>
        constexpr void emplace(const A other) {
            if (data != other) {
                modified = true;
                data = other;
            }
        }

        template<typename A>
        constexpr void emplace(HeldData<A> const& other) {
            if (data != other.getData()) {
                modified = true;
                data = other.getData();
            }
        }

        constexpr HeldData<bool>& operator=(const HeldData<bool>& other) {
            emplace(other.data);
            return *this;
        }

        constexpr HeldData<bool>& operator=(bool other) {
            emplace(other);
            return *this;
        }

        constexpr bool const& operator *() const noexcept {
            return data;
        }

        constexpr bool const& operator ->() const noexcept {
            return data;
        }

    protected:
        bool modified = false;
        bool data;
    };
    static_assert(HeldDataCheck<HeldData<bool>>);

    template<>
    struct HeldData<std::string> {
        HeldData() = default;

        explicit(false) HeldData(const std::string_view d) : data(d) {}

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

        template<typename A>
        constexpr void emplace(const A other) {
            if (data != other) {
                modified = true;
                data = other;
            }
        }

        template<typename A>
        constexpr void emplace(HeldData<A> const& other) {
            if (data != other.getData()) {
                modified = true;
                data = other.getData();
            }
        }

        constexpr HeldData<std::string>& operator=(const std::string_view other) {
            emplace(other);
            return *this;
        }

        constexpr HeldData<std::string>& operator=(const HeldData<std::string>& other) {
            emplace(other);
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, std::string>)
        constexpr HeldData<std::string>& operator=(const HeldData<U>& other) {
            emplace(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, std::string>)
        constexpr HeldData<std::string>& operator=(const U& other) {
            emplace<U>(other);
            return *this;
        }

        constexpr std::string const& operator ->() const noexcept {
            return data;
        }

        constexpr std::string const& operator *() const noexcept {
            return data;
        }

    protected:
        bool modified = false;
        std::string data;
    };
    static_assert(HeldDataCheck<HeldData<std::string>>);

    /**
     * Held data that is diff checked to a render ctx
     * @tparam T
     */
    template<class T>
    struct RenderHeldData;

//    template<typename T>
//    size_t HeldDataHash(T const& t);
//
//    template<typename T>
//    size_t HeldDataHash(T const& t) {
//        static std::hash<T> hash;
//        return hash(t);
//    }
//
//    template<typename T>
//    size_t HeldDataHash(std::optional<T> const& t) {
//        static std::hash<T> hash;
//        return hash(t);
//    }

    template<class T>
    struct RenderHeldData : public HeldData<T> {
        const Key key;

        using ParentType = HeldData<T>;

        RenderHeldData() = default;
        RenderHeldData(RenderHeldData<T> const& d) = default;
        RenderHeldData(ParentType const& d) : ParentType(d) {}
        RenderHeldData(T const& d) : ParentType(d) {}

        template<class U>
        RenderHeldData(U const& d) : HeldData<T>(d) {}

        constexpr RenderHeldData<T>& operator=(const T& other) {
            emplace<T>(other);
            return *this;
        }

        template<class U>
        constexpr RenderHeldData<T>& operator=(const U& other) {
            emplace<U>(other);
            return *this;
        }

        template<class U>
        constexpr RenderHeldData<T>& operator=(const HeldData<U>& other) {
            emplace<U>(other.getData());
            return *this;
        }

        template<class U>
        constexpr RenderHeldData<T>& operator=(const RenderHeldData<U>& other) {
            emplace<U>(other.getData());
            return *this;
        }

        template<class U>
        constexpr RenderHeldData<T>& operator=(const ParentType& other) {
            emplace<U>(other.getData());
            return *this;
        }

        template<typename A>
        constexpr void emplace(A const& other) {
            ParentType::data = other;
        }

        /**
         *
         * @param ctx
         * @return true if does not exist in RenderCtx or is different
         */
         template<bool trueIfExist = true>
         [[nodiscard]] bool isRenderDiffModified(RenderContext const& ctx) const {
            auto found = ctx.findChildData<false>(key);
            if (!found.has_value()) return trueIfExist;

            if (!found->get().hasData()) return true;

            size_t renderData = found->get().getData<size_t>();

            std::hash<T> hash;
            return renderData != hash(HeldData<T>::data);
        }

        constexpr void markCleanForRender(RenderContext& ctx) const {
            std::hash<T> hash;

            ctx.getChildDataOrCreate(key).getData<size_t>() = hash(HeldData<T>::data);
        }

        [[nodiscard]] bool readAndClear(RenderContext& ctx) const {
            bool modified = isRenderDiffModified(ctx);
            if (modified) {
                markCleanForRender(ctx);
            }
            return modified;
        }


    private:
        [[deprecated("isModified is not intended for RenderHeldData")]]
        explicit(false) constexpr operator bool() const noexcept {return ParentType::operator bool();}

        [[deprecated("isModified is not intended for RenderHeldData")]]
        [[nodiscard]] constexpr bool isModified() const noexcept {
            return ParentType::isModified();
        }

        [[deprecated("Clear is not intended for RenderHeldData")]]
        constexpr void clear() noexcept {
            ParentType::clear();
        }
    };
}