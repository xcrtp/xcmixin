/*
MSVC Compiler Bug: Overload Resolution Failure

Conditions:
- Compiler: MSVC only (GCC/Clang unaffected)
- Affected Macros: XCMIXIN_IMPL_EXTEND_FOR (when extending existing methods)
- Class Structure: Derived class introduces base overloads via using Base::func
- Validation: xcmixin_no_hiding macro checks for method hiding
- Trigger Phase: static_assert during compile-time validation (no runtime impact)
*/

#include <type_traits>

struct A {
    int hello(int) { return 42; }
    int hello() { return 42; }
};

struct B : A {
    using A::hello;
};

struct C : B {};

template <typename C>
struct fnn {
    template <typename BaseClass, typename R>
    static constexpr auto of(R (BaseClass::*f)(int))
        requires(std::is_base_of_v<BaseClass, C> ||
                 std::is_same_v<BaseClass, C>)
    {
        return f;
    }
};

int main() {
    fnn<C>::of(&C::hello);  // MSVC fails, other compilers work
}