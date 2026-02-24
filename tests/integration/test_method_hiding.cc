// Integration test: Method hiding validation
#include <iostream>
#include <string>

#include "xcmixin/xcmixin.hpp"

// Test 1: No hiding case - derived class method matches base
XCMIXIN_PRE_DECL(no_hiding_mixin)
XCMIXIN_REQUIRE(no_hiding_mixin, xcmixin_no_hiding(method, int);)

XCMIXIN_DEF_BEGIN(no_hiding_mixin)
int method(int x) { return x * 2; }
XCMIXIN_DEF_END()

class NoHidingClass;
XCMIXIN_IMPL_AVAILABLE(NoHidingClass);
XCMIXIN_IMPL_BEGIN(no_hiding_mixin)
XCMIXIN_IMPL_FOR(NoHidingClass)
int method(int x) { return x * 3; }
XCMIXIN_IMPL_END()

class NoHidingClass
    : public xcmixin::impl_recorder<NoHidingClass,
                                    xcmixin::mixin_recorder<no_hiding_mixin>> {
    xcmixin_init_class;
};

// Test 2: Const method only - using the correct syntax
XCMIXIN_PRE_DECL(const_method_mixin)
XCMIXIN_REQUIRE(const_method_mixin, )

XCMIXIN_DEF_BEGIN(const_method_mixin)
int data() const { return 2; }
XCMIXIN_DEF_END()

class ConstMethodClass;
XCMIXIN_IMPL_AVAILABLE(ConstMethodClass);
XCMIXIN_IMPL_BEGIN(const_method_mixin)
XCMIXIN_IMPL_FOR(ConstMethodClass)
int data() const { return 20; }
XCMIXIN_IMPL_END()

class ConstMethodClass
    : public xcmixin::impl_recorder<
          ConstMethodClass, xcmixin::mixin_recorder<const_method_mixin>> {
    xcmixin_init_class;
};

// Test 3: Void method
XCMIXIN_PRE_DECL(void_method_mixin)
XCMIXIN_REQUIRE(void_method_mixin, )

XCMIXIN_DEF_BEGIN(void_method_mixin)
void process() {}
XCMIXIN_DEF_END()

class VoidMethodClass;
XCMIXIN_IMPL_AVAILABLE(VoidMethodClass);
XCMIXIN_IMPL_BEGIN(void_method_mixin)
XCMIXIN_IMPL_FOR(VoidMethodClass)
void process() {}
XCMIXIN_IMPL_END()

class VoidMethodClass
    : public xcmixin::impl_recorder<
          VoidMethodClass, xcmixin::mixin_recorder<void_method_mixin>> {
    xcmixin_init_class;
};

// Compile-time verification: static_asserts for method hiding prevention
static_assert(xcmixin::is_impl<NoHidingClass, no_hiding_mixin>,
              "NoHidingClass has no_hiding_mixin");
static_assert(xcmixin::is_impl<ConstMethodClass, const_method_mixin>,
              "ConstMethodClass has const_method_mixin");
static_assert(xcmixin::is_impl<VoidMethodClass, void_method_mixin>,
              "VoidMethodClass has void_method_mixin");

int main() {
    NoHidingClass no_hide;
    ConstMethodClass const_method;
    VoidMethodClass void_method;

    // Runtime verification
    if (no_hide.method(5) != 15) {
        std::cerr << "Failed: NoHidingClass::method" << std::endl;
        return 1;
    }

    if (const_method.data() != 20) {
        std::cerr << "Failed: ConstMethodClass::data" << std::endl;
        return 2;
    }

    std::cout << "Method hiding validation test passed!" << std::endl;
    return 0;
}
