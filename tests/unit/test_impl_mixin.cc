// Unit tests for impl_mixin and impl_recorder
#include <type_traits>
#include "xcmixin/xcmixin.hpp"

// Test mixin declarations
XCMIXIN_PRE_DECL(mixin_a)
XCMIXIN_PRE_DECL(mixin_b)
XCMIXIN_PRE_DECL(mixin_c)

// Mixin definitions
XCMIXIN_DEF_BEGIN(mixin_a)
int get_a() const { return 1; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(mixin_b)
int get_b() const { return 2; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(mixin_c)
int get_c() const { return 3; }
XCMIXIN_DEF_END()

// Test: is_impl checks if derived implements mixins
using recorder = xcmixin::mixin_recorder<mixin_a, mixin_b>;
class DerivedClass;
XCMIXIN_IMPL_AVAILABLE(DerivedClass);
XCMIXIN_IMPL_BEGIN(mixin_a)
XCMIXIN_IMPL_FOR(DerivedClass)
int get_a() const { return 1; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(mixin_b)
XCMIXIN_IMPL_FOR(DerivedClass)
int get_b() const { return 2; }
XCMIXIN_IMPL_END()

class DerivedClass : public xcmixin::impl_recorder<DerivedClass, recorder> {
    xcmixin_init_class;
};

static_assert(xcmixin::has_mixin<mixin_a, recorder>, "has_mixin finds first mixin");
static_assert(xcmixin::has_mixin<mixin_b, recorder>, "has_mixin finds second mixin");
static_assert(!xcmixin::has_mixin<mixin_c, recorder>, "has_mixin returns false for missing");
static_assert(xcmixin::is_impl<DerivedClass, mixin_a>, "is_impl finds first mixin");
static_assert(xcmixin::is_impl<DerivedClass, mixin_b>, "is_impl finds second mixin");
static_assert(!xcmixin::is_impl<DerivedClass, mixin_c>, "is_impl returns false for missing");

// Test: Impl concept works
static_assert(xcmixin::Impl<DerivedClass, mixin_a, mixin_b>, "Impl concept works");

// Test: class_size works for known types
static_assert(xcmixin::class_size<int> == sizeof(int), "class_size works for int");
static_assert(xcmixin::class_size<double> == sizeof(double), "class_size works for double");

int main() {
    // All tests use static_assert, so if we reach here, all tests passed
    DerivedClass d;
    (void)d;  // suppress unused variable warning

    // Runtime verification
    if (d.get_a() != 1) return 1;
    if (d.get_b() != 2) return 2;

    return 0;
}
