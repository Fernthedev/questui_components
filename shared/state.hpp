#pragma once

#include <concepts>
#include <tuple>
#include <string_view>
#include <string>
#include <type_traits>
#include <optional>

#include "key.hpp"
#include "context.hpp"

namespace QUC {

    template<class T>
    struct HeldData;

    template<class T>
    struct BaseHeldData;

    template<class T>
    /// @brief A concept for updatable components.
    /// @tparam T The type to check.
    concept HeldDataCheck = requires (T const t) {
        {t.isModified()} noexcept -> std::same_as<bool>;
        {t.getData()} noexcept;// -> std::convertible_to<T>;
        {*t} noexcept; //-> std::same_as<T const&>;
        {t.operator ->()} noexcept; // -> std::convertible_to<T const&>;
        {(bool) t};

        std::is_default_constructible_v<T>;

        // TODO: How to static assert more constructors, constexpr and const
    } && requires(T t) {
        {t.clear()} noexcept;
    };

    template<class T>
    struct BaseHeldData {
        constexpr BaseHeldData() = default;
        constexpr ~BaseHeldData() = default;

        constexpr BaseHeldData(BaseHeldData<T> const&) = default;
        constexpr BaseHeldData(BaseHeldData<T>&&) = default;
        constexpr BaseHeldData(T const& data) : data(data) {}

        template<class U>
        requires (std::is_convertible_v<U, T> || std::is_constructible_v<T, U>)
        constexpr explicit(false) BaseHeldData(const BaseHeldData<U>& other) : modified(other.modified), data(other.data) {}

        template<class Q>
        requires (std::is_convertible_v<Q, T> || std::is_constructible_v<T, Q>)
        constexpr explicit(false) BaseHeldData(Q&& arg) : data(std::forward<Q>(arg)) {}

        template<typename... TArgs>
        requires (std::is_constructible_v<T, TArgs...>)
        constexpr explicit(false) BaseHeldData(TArgs&&... arg) : data(std::forward<TArgs>(arg)...) {}

        constexpr explicit(false) operator T () const noexcept requires(!std::is_same_v<T, bool>) {return data;}

        constexpr explicit(false) operator T const&() const noexcept requires(!std::is_same_v<T, bool>) {return data;}


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
        constexpr void emplace(BaseHeldData<A> const& other) {
            if (data != other.getData()) {
                modified = true;
                data = other.getData();
            }
        }

        constexpr BaseHeldData<T>& operator=(const T& other) {
            emplace<T>(other);
            return *this;
        }

        constexpr BaseHeldData<T>& operator=(const BaseHeldData<T>& other) {
            emplace(other);
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr BaseHeldData<T>& operator=(const BaseHeldData<U>& other) {
            emplace(other);
            return *this;
        }
        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr BaseHeldData<T>& operator=(const U& other) {
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

    template<class T>
    struct HeldData : public BaseHeldData<T> {
        using ParentType = BaseHeldData<T>;

        constexpr HeldData() = default;
        constexpr ~HeldData() = default;

        constexpr HeldData(HeldData const&) = default;
        constexpr HeldData(HeldData&&) noexcept = default;
        constexpr HeldData(T const& data) : ParentType(data) {}

        template<class U>
        constexpr explicit(false) HeldData(const BaseHeldData<U>& other) : ParentType(other) {}

        template<class Q>
        constexpr explicit(false) HeldData(Q&& arg) : ParentType(std::forward<Q>(arg)) {}

        template<typename... TArgs>
        constexpr explicit(false) HeldData(TArgs&&... arg) : ParentType(std::forward<TArgs>(arg)...) {}

        constexpr HeldData& operator=(HeldData const& other) {
            ParentType::template emplace<T>(other);
            return *this;
        };
    };
    static_assert(HeldDataCheck<HeldData<int>>);

    template<class T>
    struct HeldData<std::optional<T>> : public BaseHeldData<std::optional<T>> {
        using ParentType = BaseHeldData<std::optional<T>>;

        constexpr HeldData() = default;
        constexpr ~HeldData() = default;
        constexpr HeldData(HeldData const&) = default;
        constexpr HeldData(HeldData&&) = default;

        constexpr HeldData(T const& data) : ParentType(data) {}
        constexpr HeldData(std::optional<T> const& data) : HeldData(data) {}

        template<class U>
        constexpr explicit(false) HeldData(const HeldData<U>& other) : HeldData(other) {}

        template<class U>
        constexpr explicit(false) HeldData(const std::optional<U>& other) : HeldData(other) {}

        template<class U>
        constexpr explicit(false) HeldData(const BaseHeldData<U>& other) : ParentType(other) {}

        template<class Q>
        constexpr explicit(false) HeldData(Q&& arg) : ParentType(std::forward<Q>(arg)) {}

        template<typename... TArgs>
        constexpr explicit(false) HeldData(TArgs&&... arg) : ParentType(std::forward<TArgs>(arg)...) {}
        //

        constexpr HeldData& operator=(HeldData const& other) {
            ParentType::template emplace<T>(other);
            return *this;
        };

        constexpr HeldData<std::optional<T>>& operator=(const T& other) {
            ParentType::emplace(other);
            return *this;
        }

        constexpr HeldData<std::optional<T>>& operator=(const HeldData<T>& other) {
            ParentType::emplace(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const HeldData<U>& other) {
            ParentType::emplace(other);
            return *this;
        }

        template<class U>
        requires (std::is_convertible_v<U, T>)
        constexpr HeldData<std::optional<T>>& operator=(const U& other) {
            ParentType::template emplace<U>(other);
            return *this;
        }
    };
    static_assert(HeldDataCheck<HeldData<std::optional<int>>>);
    static_assert(HeldDataCheck<HeldData<std::optional<bool>>>);

    template<>
    struct HeldData<bool> : public BaseHeldData<bool> {
        using ParentType = BaseHeldData<bool>;

        constexpr HeldData() = default;
        constexpr ~HeldData() = default;

        constexpr HeldData(HeldData const&) = default;
        constexpr HeldData(HeldData&&) = default;
        constexpr HeldData(bool data) : ParentType(data) {};

        template<class U>
        constexpr explicit(false) HeldData(const BaseHeldData<U>& other) : ParentType(other) {}

        template<class Q>
        constexpr explicit(false) HeldData(Q&& arg) : ParentType(std::forward<Q>(arg)) {}

        template<typename... TArgs>
        constexpr explicit(false) HeldData(TArgs&&... arg) : ParentType(std::forward<TArgs>(arg)...) {}

        constexpr HeldData<bool>& operator=(bool other) {
            ParentType::emplace(other);
            return *this;
        }

        constexpr HeldData& operator=(HeldData const& other) {
            ParentType::template emplace<bool>(other);
            return *this;
        };
    };
    static_assert(HeldDataCheck<HeldData<bool>>);

    template<>
    struct HeldData<std::string> : public BaseHeldData<std::string> {
        using ParentType = BaseHeldData<std::string>;

        constexpr HeldData() = default;
        ~HeldData() = default;

        HeldData(HeldData const&) = default;
        HeldData(HeldData&&) = default;


        constexpr HeldData(std::string const& data) : ParentType(data) {}
        explicit(false) HeldData(const std::string_view d) : BaseHeldData(d) {}

        template<size_t sz>
        constexpr explicit(false) HeldData(const char (&str)[sz]) : BaseHeldData(str) {}

        ///
        template<class U>
        constexpr explicit(false) HeldData(const BaseHeldData<U>& other) : ParentType(other) {}

        template<class Q>
        constexpr explicit(false) HeldData(Q&& arg) : ParentType(std::forward<Q>(arg)) {}

        template<typename... TArgs>
        constexpr explicit(false) HeldData(TArgs&&... arg) : ParentType(std::forward<TArgs>(arg)...) {}

        constexpr HeldData<std::string>& operator=(const std::string_view other) {
            ParentType::template emplace(other);
            return *this;
        }

        constexpr HeldData& operator=(HeldData const& other) {
            ParentType::template emplace<std::string>(other);
            return *this;
        };
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

    enum struct DiffModifyCheck {
        // True if the tree contains the key and is different
        TRUE_WHEN_FOUND,
        // true if the tree does not contain the tree or contains the key and is different.
        TRUE_WHEN_NOT_FOUND,
        // true if the tree does not contain the tree and was assigned a value or contains the key and is different
        TRUE_WHEN_FOUND_OR_ASSIGNED
    };

    template<class T>
    struct RenderHeldData : public HeldData<T> {
        const Key key;

        using ParentType = HeldData<T>;

        constexpr RenderHeldData() = default;
        constexpr ~RenderHeldData() = default;
        constexpr RenderHeldData(RenderHeldData<T> const& d) = default;
        constexpr RenderHeldData(RenderHeldData<T>&& d) = default;

        constexpr RenderHeldData(ParentType const& d) : ParentType(d) {
            ParentType::modified = true;
        }
        constexpr RenderHeldData(T const& d) : ParentType(d) {
            ParentType::modified = true;
        }

        constexpr RenderHeldData(T&& d) : ParentType(d) {
            ParentType::modified = true;
        }

        template<class U>
        constexpr RenderHeldData(U const& d) : ParentType(d) {
            ParentType::modified = true;
        }

        template<class U>
        constexpr RenderHeldData(RenderHeldData<U> const& d) : ParentType(d) {
            ParentType::modified = true;
        }

        template<class U>
        constexpr RenderHeldData(HeldData<U> const& d) : ParentType(d) {
            ParentType::modified = true;
        }

        template<class U>
        constexpr RenderHeldData(U&& d) : ParentType(d) {
            ParentType::modified = true;
        }

        template<typename... TArgs>
        constexpr RenderHeldData(TArgs&&... args) : ParentType(std::forward<TArgs>(args)...) {
            ParentType::modified = true;
        }


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
        constexpr RenderHeldData<T>& operator=(const ParentType& other) {
            emplace<U>(other.getData());
            return *this;
        }

        template<typename A>
        constexpr void emplace(A const& other) {
            ParentType::data = other;
            ParentType::modified = true;
        }

        /**
         *
         * @param ctx
         * @return true if does not exist in RenderCtx or is different
         */
        template<DiffModifyCheck modifyCheck = DiffModifyCheck::TRUE_WHEN_NOT_FOUND>
        [[nodiscard]] bool isRenderDiffModified(RenderContext const& ctx) const {
            auto found = ctx.findChildData<false>(key);
            if (!found.has_value()) {
                if (modifyCheck == DiffModifyCheck::TRUE_WHEN_FOUND_OR_ASSIGNED) {
                    return ParentType::isModified();
                }
                return modifyCheck == DiffModifyCheck::TRUE_WHEN_NOT_FOUND;
            }

            if (!found->get().hasData()) return true;

            size_t renderData = found->get().getData<size_t>();

            std::hash<T> hash;
            return renderData != hash(HeldData<T>::data);
        }

        constexpr void markCleanForRender(RenderContext& ctx) const {
            std::hash<T> hash;

            ctx.getChildDataOrCreate(key).getData<size_t>() = hash(HeldData<T>::data);
        }

        template<DiffModifyCheck modifyCheck = DiffModifyCheck::TRUE_WHEN_NOT_FOUND>
        [[nodiscard]] constexpr bool readAndClear(RenderContext& ctx) const {
            bool modified = isRenderDiffModified<modifyCheck>(ctx);
            if (modified) {
                markCleanForRender(ctx);
            }
            return modified;
        }

        [[deprecated("isModified is not intended for RenderHeldData")]]
        [[nodiscard]] constexpr bool isModified() const noexcept {
            return ParentType::isModified();
        }

    private:
        [[deprecated("isModified is not intended for RenderHeldData")]]
        explicit(false) constexpr operator bool() const noexcept {return ParentType::operator bool();}


        [[deprecated("Clear is not intended for RenderHeldData")]]
        constexpr void clear() noexcept {
            ParentType::clear();
        }
    };
}