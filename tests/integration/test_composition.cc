// Integration test: Multi-layer mixin composition
#include <iostream>
#include <string>
#include "xcmixin/xcmixin.hpp"

// Layer 1: Base mixins
XCMIXIN_PRE_DECL(name_mixin)
XCMIXIN_DEF_BEGIN(name_mixin)
std::string get_name() const { return "BaseName"; }
XCMIXIN_DEF_END()

XCMIXIN_PRE_DECL(id_mixin)
XCMIXIN_DEF_BEGIN(id_mixin)
int get_id() const { return 0; }
XCMIXIN_DEF_END()

// Layer 2: Mixin with simple output
XCMIXIN_PRE_DECL(display_mixin)
XCMIXIN_REQUIRE(display_mixin,
    xcmixin_require_method(get_name, void);
    xcmixin_require_method(get_id, void);)

XCMIXIN_DEF_BEGIN(display_mixin)
void display() const {
    std::cout << "display mixin called" << std::endl;
}
XCMIXIN_DEF_END()

// Test class with base mixins only
class BasicClass;
XCMIXIN_IMPL_AVAILABLE(BasicClass);
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(BasicClass)
std::string get_name() const { return "BasicClass"; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(id_mixin)
XCMIXIN_IMPL_FOR(BasicClass)
int get_id() const { return 1; }
XCMIXIN_IMPL_END()

using BasicRecorder = xcmixin::mixin_recorder<name_mixin, id_mixin>;
class BasicClass : public xcmixin::impl_recorder<BasicClass, BasicRecorder> {
    xcmixin_init_class;
};

// Test class with all mixins (full composition)
class FullClass;
XCMIXIN_IMPL_AVAILABLE(FullClass);

XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(FullClass)
std::string get_name() const { return "FullClass"; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(id_mixin)
XCMIXIN_IMPL_FOR(FullClass)
int get_id() const { return 3; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(display_mixin)
XCMIXIN_IMPL_FOR(FullClass)
void display() const {
    std::cout << "FullClass display" << std::endl;
}
XCMIXIN_IMPL_END()

using FullRecorder = xcmixin::mixin_recorder<name_mixin, id_mixin, display_mixin>;
class FullClass : public xcmixin::impl_recorder<FullClass, FullRecorder> {
    xcmixin_init_class;
};

// Test: Verify inheritance chain
static_assert(xcmixin::is_impl<BasicClass, name_mixin>, "BasicClass has name_mixin");
static_assert(xcmixin::is_impl<BasicClass, id_mixin>, "BasicClass has id_mixin");
static_assert(xcmixin::is_impl<FullClass, display_mixin>, "FullClass has display_mixin");

// Test: Concept constraints
static_assert(xcmixin::Impl<BasicClass, name_mixin, id_mixin>,
              "BasicClass implements required mixins");
static_assert(xcmixin::Impl<FullClass, name_mixin, id_mixin, display_mixin>,
              "FullClass implements required mixins");

int main() {
    BasicClass basic;
    FullClass full;

    // Runtime tests
    full.display();

    // Verify method calls work
    if (basic.get_id() != 1) return 1;
    if (full.get_id() != 3) return 2;

    std::cout << "Multi-layer composition test passed!" << std::endl;
    return 0;
}
