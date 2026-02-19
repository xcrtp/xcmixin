#pragma once
#include <type_traits>

namespace xcmixin {

// basic types
namespace details {

template <typename emitter>
struct invalid_type {
    static_assert(false, "invalid type");
};
template <typename emitter, typename T = bool>
struct invalid_value_type {
    static_assert(false, "invalid type");
    static constexpr bool value = T{};
};
template <typename emitter, typename T = bool>
constexpr static T invalid_value = invalid_value_type<emitter, T>::value;

template <typename T>
struct return_type {
    using type = T;
};
template <typename T>
using deref_type = typename T::type;

namespace fn {
template <typename... elms>
struct type_list;

template <typename container, typename... elms>
struct push_back_helper : invalid_value_type<container> {};
template <typename container, typename... elms>
using push_back = deref_type<push_back_helper<container, elms...>>;
template <template <typename...> typename container, typename... oelms,
          typename... elms>
struct push_back_helper<container<oelms...>, elms...>
    : return_type<container<oelms..., elms...>> {};
template <typename container, typename... elms>
struct push_front_helper : invalid_value_type<container> {};
template <typename container, typename... elms>
using push_front = deref_type<push_front_helper<container, elms...>>;
template <template <typename...> typename container, typename... oelms,
          typename... elms>
struct push_front_helper<container<oelms...>, elms...>
    : return_type<container<elms..., oelms...>> {};
template <typename... containers>
struct concat_helper : invalid_value_type<type_list<containers...>> {};
template <typename... containers>
using concat = deref_type<concat_helper<containers...>>;
template <template <typename...> typename container, typename... oelms,
          typename... elms>
struct concat_helper<container<oelms...>, container<elms...>>
    : return_type<container<elms..., oelms...>> {};
template <typename container>
struct concat_helper<container> : return_type<container> {};
template <typename container1, typename container2, typename... containers>
struct concat_helper<container1, container2, containers...>
    : return_type<concat<concat<container1, container2>, containers...>> {};
template <typename container>
static constexpr bool is_empty = invalid_value<container>;
template <template <typename...> typename container, typename... elms>
static constexpr bool is_empty<container<elms...>> = sizeof...(elms) == 0;
template <typename T, typename container>
static constexpr bool is_one_of = invalid_value<container>;
template <typename T, template <typename...> typename container>
static constexpr bool is_one_of<T, container<>> = false;
template <typename T, template <typename...> typename container, typename... O>
static constexpr bool is_one_of<T, container<O...>> =
    (std::is_same_v<T, O> || ...);

template <typename container, typename T>
static constexpr bool contains = is_one_of<T, container>;

}  // namespace fn
}  // namespace details

// overload validator
namespace details {
// basic overload types
template <typename... Owner>
struct member_owner;
template <typename... Args>
struct args;
template <typename... Args>
struct ret;

// member category
namespace member_category {
struct any_;
struct const_;
struct volatile_;
struct const_volatile_;
struct non_const_volatile_;
struct static_;

using category_list = fn::type_list<any_, const_, volatile_, const_volatile_,
                                    non_const_volatile_, static_>;
template <typename T>
static constexpr bool is_category = fn::contains<category_list, T>;
}  // namespace member_category

// template method validator
template <typename Derived, typename Base, typename expected_return_type,
          typename return_type>
static constexpr bool is_valid =
    (std::is_base_of_v<Base, Derived> || std::is_same_v<Base, Derived>) &&
    (fn::contains<expected_return_type, return_type> ||
     fn::is_empty<expected_return_type>);
template <typename... Args>
inline constexpr bool is_void_any = (std::is_void_v<Args> || ...);
template <>
inline constexpr bool is_void_any<> = false;
template <typename... Args>
inline constexpr bool is_nomal = (sizeof...(Args) > 0 && !is_void_any<Args...>);
template <>
inline constexpr bool is_nomal<> = false;
template <typename... Args>
inline constexpr bool is_empty = sizeof...(Args) == 0;

// overloader
template <typename args = args<>, typename owner = member_owner<>,
          typename category = member_category::any_, typename ret = ret<>>
struct overloader : invalid_type<fn::type_list<args, owner, category, ret>> {};
template <typename Derived, typename... R, typename... Args>
struct overloader<args<Args...>, member_owner<Derived>,
                  member_category::non_const_volatile_, ret<R...>> {
    using category = member_category::non_const_volatile_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)(Args...))
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_nomal<Args...>)
    {
        return f;
    }
    template <typename Base, typename R_, typename... Args_>
    static consteval auto of(R_ (Base::*f)(Args_...))
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_empty<Args...>)
    {
        return f;
    }
};

template <typename Derived, typename... R, typename... Args>
struct overloader<args<Args...>, member_owner<Derived>, member_category::const_,
                  ret<R...>> {
    using category = member_category::const_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)(Args...) const)
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_nomal<Args...>)
    {
        return f;
    }
    template <typename Base, typename R_, typename... Args_>
    static consteval auto of(R_ (Base::*f)(Args_...) const)
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_empty<Args...>)
    {
        return f;
    }
};
template <typename Derived, typename... R, typename... Args>
struct overloader<args<Args...>, member_owner<Derived>,
                  member_category::volatile_, ret<R...>> {
    using category = member_category::volatile_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)(Args...) volatile)
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_nomal<Args...>)
    {
        return f;
    }
    template <typename Base, typename R_, typename... Args_>
    static consteval auto of(R_ (Base::*f)(Args_...) volatile)
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_empty<Args...>)
    {
        return f;
    }
};
template <typename Derived, typename... R, typename... Args>
struct overloader<args<Args...>, member_owner<Derived>,
                  member_category::const_volatile_, ret<R...>> {
    using category = member_category::const_volatile_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)(Args...) const volatile)
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_nomal<Args...>)
    {
        return f;
    }
    template <typename Base, typename R_, typename... Args_>
    static consteval auto of(R_ (Base::*f)(Args_...) const volatile)
        requires(is_valid<Derived, Base, ret<R...>, R_> && is_empty<Args...>)
    {
        return f;
    }
};
template <typename Derived, typename... R, typename... Args>
struct overloader<args<Args...>, member_owner<Derived>,
                  member_category::static_, ret<R...>> {
    using category = member_category::static_;
    template <typename R_>
    static consteval auto of(R_ (*f)(Args...))
        requires(is_valid<Derived, Derived, ret<R...>, R_> && is_nomal<Args...>)
    {
        return f;
    }
    template <typename R_, typename... Args_>
    static consteval auto of(R_ (*f)(Args_...))
        requires(is_valid<Derived, Derived, ret<R...>, R_> && is_empty<Args...>)

    {
        return f;
    }
};
template <typename Derived, typename... R>
struct overloader<args<void>, member_owner<Derived>, member_category::const_,
                  ret<R...>> {
    using category = member_category::const_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)() const)
        requires(is_valid<Derived, Base, ret<R...>, R_>)
    {
        return f;
    }
};
template <typename Derived, typename... R>
struct overloader<args<void>, member_owner<Derived>, member_category::volatile_,
                  ret<R...>> {
    using category = member_category::volatile_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)() volatile)
        requires(is_valid<Derived, Base, ret<R...>, R_>)

    {
        return f;
    }
};
template <typename Derived, typename... R>
struct overloader<args<void>, member_owner<Derived>,
                  member_category::non_const_volatile_, ret<R...>> {
    using category = member_category::non_const_volatile_;
    template <typename Base, typename R_>
    static consteval auto of(R_ (Base::*f)())
        requires(is_valid<Derived, Base, ret<R...>, R_>)

    {
        return f;
    }
};
template <typename Derived, typename... R>
struct overloader<args<void>, member_owner<Derived>, member_category::static_,
                  ret<R...>> {
    using category = member_category::static_;
    template <typename R_>
    static consteval auto of(R_ (*f)())
        requires(is_valid<Derived, Derived, ret<R...>, R_>)

    {
        return f;
    }
};
template <typename Derived, typename... R, typename... Args>
struct overloader<args<Args...>, member_owner<Derived>, member_category::any_,
                  ret<R...>>
    : overloader<args<Args...>, member_owner<Derived>, member_category::const_,
                 ret<R...>>,
      overloader<args<Args...>, member_owner<Derived>,
                 member_category::volatile_, ret<R...>>,
      overloader<args<Args...>, member_owner<Derived>,
                 member_category::non_const_volatile_, ret<R...>>,
      overloader<args<Args...>, member_owner<Derived>, member_category::static_,
                 ret<R...>> {
    using category = member_category::any_;
    using overloader<args<Args...>, member_owner<Derived>,
                     member_category::const_, ret<R...>>::of;
    using overloader<args<Args...>, member_owner<Derived>,
                     member_category::volatile_, ret<R...>>::of;
    using overloader<args<Args...>, member_owner<Derived>,
                     member_category::non_const_volatile_, ret<R...>>::of;
    using overloader<args<Args...>, member_owner<Derived>,
                     member_category::static_, ret<R...>>::of;
};

// overload args parser
template <typename args_, typename category_, typename ret_>
struct overload_args {
    template <typename Derived>
    using overloader =
        overloader<args_, member_owner<Derived>, category_, ret_>;
};
template <typename... Args>
struct parser_overload_args_helper;
template <typename... Args>
using parser_overload_args = deref_type<parser_overload_args_helper<Args...>>;

template <typename... Args>
struct parser_overload_args_helper
    : return_type<parser_overload_args<
          overload_args<args<>, member_category::any_, ret<>>, Args...>> {};

template <typename args_, typename category_, typename ret_>
struct parser_overload_args_helper<overload_args<args_, category_, ret_>>
    : return_type<overload_args<args_, category_, ret_>> {};

template <typename args_, typename category_, typename ret_, typename category,
          typename... Args>
    requires(member_category::is_category<category>)
struct parser_overload_args_helper<overload_args<args_, category_, ret_>,
                                   category, Args...>
    : return_type<
          parser_overload_args<overload_args<args_, category, ret_>, Args...>> {
};

template <typename args_, typename category_, typename ret_, typename R,
          typename... Args>
struct parser_overload_args_helper<overload_args<args_, category_, ret_>,
                                   ret<R>, Args...>
    : return_type<parser_overload_args<overload_args<args_, category_, ret<R>>,
                                       Args...>> {};

template <typename args_, typename category_, typename ret_, typename Arg,
          typename... Args>
struct parser_overload_args_helper<overload_args<args_, category_, ret_>, Arg,
                                   Args...>
    : return_type<parser_overload_args<
          overload_args<fn::push_back<args_, Arg>, category_, ret_>, Args...>> {
};

// export overload api
template <typename... Args>
struct overload {
    template <typename owner>
    using overloader =
        parser_overload_args<Args...>::template overloader<owner>;
};

}  // namespace details

// core mixin framework
// simplify method declaration
#define METHOD                              \
    template <typename, typename, typename> \
    class

// root empty base class for inherit chain
template <typename Derived>
struct EmptyBase {
    template <typename D = Derived>
    constexpr static bool valid_class() {
        return true;
    }
};

// method common validator, all special methods will be call it to validate
template <typename meta>
struct method_validator {
    template <typename MethodClass, typename Derived>
    static consteval bool valid_method() {
        return true;
    }
};
// core framework implementation
namespace details {
// method recorder, store all methods in it
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

// meta method, store method class and method type
template <METHOD method_>
struct meta_method {
    template <typename Base, typename Derived, typename method_type>
    using method = method_<Base, Derived, method_type>;
};

// method recorder concat helper, concat all method recorders
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

// method base class validator, check if method base class is valid
template <typename Method, typename Derived, typename = void>
constexpr bool vaild_base_class = true;
template <typename Method, typename Derived>
constexpr auto
    vaild_base_class<Method, Derived, std::void_t<typename Method::base_meta>> =
        method_validator<typename Method::base_meta>::template valid_method<
            typename Method::base, Derived>() &&
        vaild_base_class<typename Method::base, Derived>;

// the core inherit chain generator, inherit impl_methods<...> to mixin all
// methods
template <typename Derived, METHOD... methods>
struct impl_methods_helper;
template <typename Derived, METHOD... methods>
using impl_methods = deref_type<impl_methods_helper<Derived, methods...>>;
template <typename Derived, METHOD method>
struct impl_methods_helper<Derived, method> {
    struct type;
    using base = method<EmptyBase<Derived>, Derived, meta_method<method>>;
    struct type : base {
        using method_recorder =
#ifdef __GNUC__
            struct
#endif
            method_recorder<method>;
        template <typename D = Derived>
        constexpr static bool valid_class() {
            return ::xcmixin::method_validator<meta_method<method>>::
                       template valid_method<base, Derived>() &&
                   vaild_base_class<base, Derived> && base::valid_class();
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
                   vaild_base_class<base, Derived> && base::valid_class();
        }
    };
};

// method recorder inherit chain generator, inherit impl_methods_recorders<...>
// to mixin all methods in the recorders
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

template <typename Derived, METHOD... method>
static constexpr bool is_impl_method =
    (... || has_method<method, typename Derived::method_recorder>);
template <typename T, typename = void>
constexpr size_t class_size = 0;
template <typename T>
constexpr size_t class_size<T, std::void_t<decltype(sizeof(T))>> = sizeof(T);

// concept, check if a class is implemented a method
template <typename T, METHOD... method>
concept Impl = is_impl_method<T, method...>;

}  // namespace details
// traits
using details::class_size;
using details::has_method;
using details::is_impl_method;
// concepts
using details::Impl;
// overload
using details::overload;
using details::ret;
using namespace details::member_category;
// methods
using details::impl_methods;
using details::impl_methods_recorders;
using details::meta_method;
using details::method_recorder;
using details::recorder_concat;

}  // namespace xcmixin

// macro expand utils
#define __XCMIXIN_PSPLITER ,
#define __XCMIXIN_PARAM_BASE(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, \
                             _12, _13, _14, _15, _16, N, ...)                  \
    N
#define __XCMIXIN_PARAM_SPIITER(...)                                \
    __XCMIXIN_PARAM_BASE(                                           \
        , ##__VA_ARGS__, __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER,    \
        __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, \
        __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, \
        __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, \
        __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, \
        __XCMIXIN_PSPLITER, __XCMIXIN_PSPLITER, )

#define __XCMIXIN_PREFIX_PARAM(...) \
    __VA_ARGS__ __XCMIXIN_PARAM_SPIITER(__VA_ARGS__)
#define __XCMIXIN_SUFIX_PARAM(...) \
    __XCMIXIN_PARAM_SPIITER(__VA_ARGS__) __VA_ARGS__
#define __XCMIXIN_MID_PARAM(...)         \
    __XCMIXIN_PARAM_SPIITER(__VA_ARGS__) \
    __VA_ARGS__ __XCMIXIN_PARAM_SPIITER(__VA_ARGS__)

// user macro api
#define XCMIXIN_METHOD_INIT()           \
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
#define XCMIXIN_METHOD_DEF_EXTEND_BEGIN(name, ext_method)          \
    template <typename Base, typename Derived, typename meta>       \
    struct name : ext_method<Base, Derived, meta> {                 \
        using Self = std::decay_t<Derived>;                         \
        using ConstSelf = const Self;                               \
        using base = ext_method<Base, Self, meta>;                  \
        using base_meta = ::xcmixin::meta_method<ext_method>;       \
        using method_recorder =                                     \
            base::method_recorder::template push_front<ext_method>; \
        using MethodClass = meta::template method<base, Self, meta>;
#define XCMIXIN_METHOD_DEF_END() \
    }                            \
    ;
#define XCMIXIN_METHOD_DECLARE(name) \
    XCMIXIN_METHOD_DEF_BEGIN(name)   \
    }

#define XCMIXIN_IMPL_METHOD_BEGIN(name, ...)                                   \
    template <typename Base, typename meta __XCMIXIN_SUFIX_PARAM(__VA_ARGS__)> \
        struct name < Base,
#define XCMIXIN_IMPL_METHOD_BEGIN_WITH_REQUIRES(name, require_statement, ...)  \
    template <typename Base, typename meta __XCMIXIN_SUFIX_PARAM(__VA_ARGS__)> \
        requires(require_statement)                                            \
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
        using base_meta = ::xcmixin::meta_method<ext_method>;       \
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
    static_assert(::xcmixin::overload<__VA_ARGS__>::template overloader<     \
                      MethodClass>::of(&MethodClass::name) ==                \
                      ::xcmixin::overload<__VA_ARGS__>::template overloader< \
                          MethodClass>::of(&Derived::name),                  \
                  "method " #name " is shadowed ")
#undef METHOD