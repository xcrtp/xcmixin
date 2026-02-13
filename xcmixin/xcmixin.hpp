#pragma once
#include <type_traits>
namespace xcmixin {
#define METHOD                              \
    template <typename, typename, typename> \
    class

struct EmptyBase {
    static constexpr bool valid_class() { return true; }
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
    using base = method<EmptyBase, Derived, meta_method<method>>;
    struct type : base {
        using method_recorder = method_recorder<method>;
    };
};
template <typename Derived, METHOD method, METHOD... methods>
struct impl_methods_helper<Derived, method, methods...> {
    struct type;
    using base = method<deref_type<impl_methods_helper<Derived, methods...>>,
                        Derived, meta_method<method>>;
    struct type : base {
        using method_recorder = method_recorder<method, methods...>;
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

}  // namespace details

using details::has_method;
using details::impl_methods;
using details::impl_methods_recorders;
using details::is_impl_method;
using details::method_recorder;
using details::recorder_concat;

}  // namespace xcmixin

#define XCMIXIN_METHOD_INIT()           \
    using base_type = Base;             \
    using Self = std::decay_t<Derived>; \
    using ConstSelf = const Self;       \
    using MethodClass = meta::template method<Base, Self, meta>;
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
#ifdef __GNUC__
#define XCMIXIN_IMPL_METHOD_BEGIN(name, ext_template_params...)  \
    template <typename Base, ext_template_params, typename meta> \
        struct name < Base,
#define XCMIXIN_IMPL_METHOD_BEGIN_WITH_REQUIRES(name, require_statement, \
                                                ext_template_params...)  \
    template <typename Base, ext_template_params, typename meta>         \
        requires(require_statement)                                      \
    struct name < Base,
#define XCMIXIN_IMPL_METHOD_FOR(cls...)                              \
    cls, meta > : Base {                                             \
        using MethodClass = meta::template method<Base, Self, meta>; \
        using Self = cls;                                            \
        using ConstSelf = const std::remove_const_t<Self>;

#else
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
#endif
#define XCMIXIN_METHOD_REQUIRES(...) \
    constexpr static bool valid_class() { \
        __VA_ARGS__                       \
        return Base::valid_class();       \
    }
#define XCMIXIN_IMPL_METHOD_END() \
    }                             \
    ;

#define XCMIXIN_FACTORY_DECLARE(name) XCMIXIN_METHOD_DECLARE(name)
#define XCMIXIN_FACTORY_DEF_BEGIN(name) XCMIXIN_METHOD_DEF_BEGIN(name)
#define XCMIXIN_FACTORY_DEF_END() XCMIXIN_METHOD_DEF_END()
#define XCMIXIN_IMPL_FACTORY_BEGIN(name, ...) \
    XCMIXIN_IMPL_METHOD_BEGIN(name, __VA_ARGS__)
#define XCMIXIN_IMPL_FACTORY_BEGIN_WITH_REQUIRES(name, require_statement, ...) \
    XCMIXIN_IMPL_METHOD_BEGIN_WITH_REQUIRES(name, require_statement,           \
                                            __VA_ARGS__)
#define XCMIXIN_IMPL_FACTORY_FOR(...) XCMIXIN_IMPL_METHOD_FOR(__VA_ARGS__)
#define XCMIXIN_IMPL_FACTORY_END() XCMIXIN_IMPL_METHOD_END()
#define xcmixin_self (*static_cast<Self*>(this))
#define xcmixin_const_self (*static_cast<ConstSelf*>(this))

#define xcmixin_require_method(method)                        \
    static_assert(::xcmixin::is_impl_method<Derived, method>, \
                  "Derived must be derived from " #method)
#define xcmixin_no_shadow(name)                      \
    static_assert(&MethodClass::name == &Self::name, \
                  "method " #name " is shadowed ")

#undef METHOD