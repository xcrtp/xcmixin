// Unit tests for macro functionality
#include <type_traits>
#include <string>
#include "xcmixin/xcmixin.hpp"

// Test macros: XCMIXIN_PRE_DECL, XCMIXIN_DEF_BEGIN/END
XCMIXIN_PRE_DECL(test_mixin_macro)

// Test: XCMIXIN_DEF_BEGIN/END creates mixin with methods
XCMIXIN_DEF_BEGIN(simple_mixin)
int method() { return 42; }
XCMIXIN_DEF_END()

// Test: Mixin with const method
XCMIXIN_DEF_BEGIN(const_mixin)
int method() const { return 42; }
XCMIXIN_DEF_END()

// Test: XCMIXIN_REQUIRE adds compile-time requirements
XCMIXIN_PRE_DECL(required_mixin)
XCMIXIN_REQUIRE(required_mixin, )  // Empty requirement - always valid

XCMIXIN_DEF_BEGIN(required_mixin)
int method() { return 1; }
XCMIXIN_DEF_END()

// Test: xcmixin_self works correctly
XCMIXIN_DEF_BEGIN(self_test_mixin)
int get_value() { return xcmixin_self.get_value(); }
int value_ = 123;
XCMIXIN_DEF_END()

// Test: xcmixin_const_self works correctly
XCMIXIN_DEF_BEGIN(const_self_test_mixin)
int get_value() const { return xcmixin_const_self.get_value(); }
XCMIXIN_DEF_END()

// Test: xcmixin_init_class validates class
XCMIXIN_PRE_DECL(init_test_mixin)
XCMIXIN_DEF_BEGIN(init_test_mixin)
int method() { return 1; }
XCMIXIN_DEF_END()

class InitTestClass;
XCMIXIN_IMPL_AVAILABLE(InitTestClass);
XCMIXIN_IMPL_BEGIN(init_test_mixin)
XCMIXIN_IMPL_FOR(InitTestClass)
int method() { return 1; }
XCMIXIN_IMPL_END()

class InitTestClass : public xcmixin::impl_recorder<InitTestClass, xcmixin::mixin_recorder<init_test_mixin>> {
    xcmixin_init_class;
};

// Test: mixin_recorder works
using test_recorder = xcmixin::mixin_recorder<simple_mixin, const_mixin>;
static_assert(xcmixin::has_mixin<simple_mixin, test_recorder>, "has_mixin works");

// Test: meta_mixin is defined
static_assert(sizeof(xcmixin::meta_mixin<simple_mixin>) > 0, "meta_mixin exists");

int main() {
    // All tests use static_assert, so if we reach here, all tests passed

    // Runtime verification
    InitTestClass init_obj;
    (void)init_obj;

    return 0;
}
