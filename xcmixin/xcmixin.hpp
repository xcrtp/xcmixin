#pragma once
#include <type_traits>
namespace xcmixin {
#define METHOD                              \
    template <typename, typename, typename> \
    class
template <typename Derived>
struct EmptyBase {
    template <typename D = Derived>
    constexpr static bool valid_class() {
        return true;
    }
};
template <typename meta>
struct method_validator {
    template <typename MethodClass, typename Derived>
    static consteval bool valid_method() {
        return true;
    }
};
namespace details {
template <typename T>
struct return_type {
    using type = T;
};
template <typename T>
using deref_type = typename T::type;

template <METHOD... methods>
struct method_recorder {
    template <METHOD... ext_methods>
    using push_back = method_recorder<methods..., ext_methods...>;

    template <METHOD... ext_methods>
    using push_front = method_recorder<ext_methods..., methods...>;
    template <typename T>
    struct concat_helper;
    template <typename T>
    using concat = deref_type<concat_helper<T>>;
    template <METHOD... ext_methods>
    struct concat_helper<method_recorder<ext_methods...>>
        : return_type<method_recorder<methods..., ext_methods...>> {};
};
template <METHOD method_>
struct meta_method {
    template <typename Base, typename Derived, typename method_type>
    using method = method_<Base, Derived, method_type>;
};
template <typename... Ts>
struct recorder_concat_helper;
template <typename... Ts>
using recorder_concat = deref_type<recorder_concat_helper<Ts...>>;

template <>
struct recorder_concat_helper<> : return_type<method_recorder<>> {};
template <typename T>
struct recorder_concat_helper<T> : return_type<T> {};
template <typename T, typename... Ts>
struct recorder_concat_helper<T, Ts...>
    : return_type<typename T::template concat<
          deref_type<recorder_concat_helper<Ts...>>>> {};

template <typename Derived, METHOD... methods>
struct impl_methods_helper;
template <typename Derived, METHOD... methods>
using impl_methods = deref_type<impl_methods_helper<Derived, methods...>>;
template <typename Derived, METHOD method>
struct impl_methods_helper<Derived, method> {
    struct type;
    using base = method<EmptyBase<Derived>, Derived, meta_method<method>>;
    struct type : base {
        using method_recorder = method_recorder<method>;
        template <typename D = Derived>
        constexpr static bool valid_class() {
            return ::xcmixin::method_validator<meta_method<method>>::
                       template valid_method<base, Derived>() &&
                   base::valid_class();
        }
    };
};
template <typename Derived, METHOD method, METHOD... methods>
struct impl_methods_helper<Derived, method, methods...> {
    struct type;
    using base = method<deref_type<impl_methods_helper<Derived, methods...>>,
                        Derived, meta_method<method>>;
    struct type : base {
        using method_recorder =
            base::method_recorder::template push_front<method>;
        template <typename D = Derived>
        constexpr static bool valid_class() {
            return ::xcmixin::method_validator<meta_method<method>>::
                       template valid_method<base, Derived>() &&
                   base::valid_class();
        }
    };
};

template <typename Derived, typename recorders>
struct impl_methods_recorder_helper;
template <typename Derived, typename... recorders>
using impl_methods_recorders = deref_type<
    impl_methods_recorder_helper<Derived, recorder_concat<recorders...>>>;
template <typename Derived, METHOD... methods>
struct impl_methods_recorder_helper<Derived, method_recorder<methods...>> {
    struct type : deref_type<impl_methods_helper<Derived, methods...>> {};
};
template <METHOD method, typename recorder>
static constexpr bool has_method = false;

template <METHOD method, METHOD... methods>
static constexpr bool has_method<method, method_recorder<methods...>> =
    (std::is_same_v<meta_method<method>, meta_method<methods>> || ...);

template <typename Derived, METHOD method>
static constexpr bool is_impl_method =
    has_method<method, typename Derived::method_recorder>;
template <typename T, typename = void>
constexpr size_t class_size = 0;
template <typename T>
constexpr size_t class_size<T, std::void_t<decltype(sizeof(T))>> = sizeof(T);

template <typename T = void>
struct overload;
template <>
struct overload<void> {
    template <typename T1, typename T2>
    static consteval auto same(T1 member1, T2 member2) {
        return member1 == member2;
    }
};
template <typename R, typename... Args>
struct overload<R(Args...)> {
    static consteval auto same(R(member1)(Args...), R(member2)(Args...)) {
        return member1 == member2;
    }
    template <typename C1, typename C2>
    static consteval auto same(R (C1::*member1)(Args...),
                               R (C2::*member2)(Args...)) {
        return member1 == member2;
    }
    template <typename C1, typename C2>
    static consteval auto same(R (C1::*member1)(Args...) const,
                               R (C2::*member2)(Args...) const) {
        return member1 == member2;
    }
};
template <typename C, typename R, typename... Args>
struct overload<R (C::*)(Args...)> {
    template <typename C1, typename C2>
    static consteval auto same(R (C1::*member1)(Args...),
                               R (C2::*member2)(Args...)) {
        return member1 == member2;
    }
};
template <typename C, typename R, typename... Args>
struct overload<R (C::*)(Args...) const> {
    template <typename C1, typename C2>
    static consteval auto same(R (C1::*member1)(Args...) const,
                               R (C2::*member2)(Args...) const) {
        return member1 == member2;
    }
};

}  // namespace details

using details::class_size;
using details::has_method;
using details::impl_methods;
using details::impl_methods_recorders;
using details::is_impl_method;
using details::meta_method;
using details::method_recorder;
using details::overload;
using details::recorder_concat;

template <typename T, METHOD method>
concept Impl = is_impl_method<T, method>;

}  // namespace xcmixin

#define XCMIXIN_METHOD_INIT()           \
    using base_type = Base;             \
    using Self = std::decay_t<Derived>; \
    using ConstSelf = const Self;       \
    using MethodClass = meta::template method<Base, Self, meta>;
#define XCMIXIN_METHOD_REQUIRE(name, ...)                   \
    namespace xcmixin {                                     \
    template <>                                             \
    struct method_validator<::xcmixin::meta_method<name>> { \
        template <typename MethodClass, typename Derived>   \
        static consteval bool valid_method() {              \
            __VA_ARGS__                                     \
            return true;                                    \
        }                                                   \
    };                                                      \
    }

#define XCMIXIN_PRE_DECL(name)                                \
    template <typename Base, typename Derived, typename meta> \
    struct name;
#define XCMIXIN_METHOD_DEF_BEGIN(name)                        \
    template <typename Base, typename Derived, typename meta> \
    struct name : Base {                                      \
        XCMIXIN_METHOD_INIT()
#define XCMIXIN_METHOD_DEF_END() \
    }                            \
    ;
#define XCMIXIN_METHOD_DECLARE(name) \
    XCMIXIN_METHOD_DEF_BEGIN(name)   \
    }

#define XCMIXIN_IMPL_METHOD_BEGIN(name, ...)             \
    template <typename Base, __VA_ARGS__, typename meta> \
        struct name < Base,
#define XCMIXIN_IMPL_METHOD_BEGIN_WITH_REQUIRES(name, require_statement, ...) \
    template <typename Base, __VA_ARGS__, typename meta>                      \
        requires(require_statement)                                           \
    struct name < Base,
#define XCMIXIN_IMPL_METHOD_FOR(...)                       \
    __VA_ARGS__, meta > : Base {                           \
        using Self = __VA_ARGS__;                          \
        using ConstSelf = const std::remove_const_t<Self>; \
        using MethodClass = meta::template method<Base, Self, meta>;
#define XCMIXIN_IMPL_METHOD_EXTEND_FOR(ext_method, ...)             \
    __VA_ARGS__, meta > : ext_method<Base, __VA_ARGS__, meta> {     \
        using Self = __VA_ARGS__;                                   \
        using ConstSelf = const std::remove_const_t<Self>;          \
        using base = ext_method<Base, Self, meta>;                  \
        using method_recorder =                                     \
            base::method_recorder::template push_front<ext_method>; \
        using MethodClass = meta::template method<base, Self, meta>;

#define XCMIXIN_METHOD_REQUIRES(...)                  \
    template <typename Derived = Self>                \
    constexpr static bool valid_class() {             \
        __VA_ARGS__                                   \
        return Base::template valid_class<Derived>(); \
    }
#define XCMIXIN_IMPL_METHOD_END() \
    }                             \
    ;
// Check whether the implementation method can be injected, only applicable to
// non-template classes
#define XCMIXIN_IMPL_AVAILABLE(name)                                          \
    static_assert(::xcmixin::class_size<name> == 0,                           \
                  "class " #name                                              \
                  " must be incomplete before impl, you must define methods " \
                  "before define class")

#define xcmixin_self (*static_cast<Self*>(this))
#define xcmixin_const_self (*static_cast<ConstSelf*>(this))
// Initialize the class, check whether the class is valid
#define xcmixin_init_class static_assert(valid_class(), "class must be valid")
// Require the method to be implemented, check whether the method is implemented
#define xcmixin_require_method(method)                        \
    static_assert(::xcmixin::is_impl_method<Derived, method>, \
                  "Derived must be derived from " #method)
// Require the method to be not shadowed, check whether the method is not
// shadowed
#define xcmixin_no_hiding(name, ...)                                         \
    static_assert(::xcmixin::overload<__VA_ARGS__>::same(&MethodClass::name, \
                                                         &Derived::name),    \
                  "method " #name " is shadowed ")
#undef METHOD