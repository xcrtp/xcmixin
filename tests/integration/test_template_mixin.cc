// Integration test: Template mixins
#include <iostream>
#include <string>
#include "xcmixin/xcmixin.hpp"

// Template mixin with type parameter - just use the pattern from examples
template <typename T>
class MyTemplate;

// Template mixin definition
XCMIXIN_PRE_DECL(template_name_mixin)
XCMIXIN_REQUIRE(template_name_mixin, )

XCMIXIN_DEF_BEGIN(template_name_mixin)
std::string get_name() const { return "template_default"; }
XCMIXIN_DEF_END()

// Non-template mixin
XCMIXIN_PRE_DECL(static_mixin)
XCMIXIN_DEF_BEGIN(static_mixin)
int get_static_value() const { return 42; }
XCMIXIN_DEF_END()

// Test: Template class with template mixin
template <typename T>
class MyTemplate;

template <typename T>
using template_recorder = xcmixin::mixin_recorder<template_name_mixin>;

template <typename T>
class MyTemplate : public xcmixin::impl_recorder<MyTemplate<T>, template_recorder<T>> {
    xcmixin_init_template(
        xcmixin::impl_recorder<MyTemplate<T>, template_recorder<T>>);
};

// Specialization for int
XCMIXIN_IMPL_BEGIN(template_name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<int>)
std::string get_name() const { return "template_int"; }
XCMIXIN_IMPL_END()

// Specialization for float
XCMIXIN_IMPL_BEGIN(template_name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<float>)
std::string get_name() const { return "template_float"; }
XCMIXIN_IMPL_END()

// Test: Mixed template and non-template mixins
template <typename T>
class MixedTemplate;

template <typename T>
using mixed_recorder = xcmixin::mixin_recorder<template_name_mixin, static_mixin>;

template <typename T>
class MixedTemplate
    : public xcmixin::impl_recorder<MixedTemplate<T>, mixed_recorder<T>> {
    xcmixin_init_template(
        xcmixin::impl_recorder<MixedTemplate<T>, mixed_recorder<T>>);
};

// Mixed specialization for int
XCMIXIN_IMPL_BEGIN(template_name_mixin)
XCMIXIN_IMPL_FOR(MixedTemplate<int>)
std::string get_name() const { return "mixed_int"; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(static_mixin)
XCMIXIN_IMPL_FOR(MixedTemplate<int>)
int get_static_value() const { return 99; }
XCMIXIN_IMPL_END()

// Compile-time verification: template mixin instantiation
static_assert(xcmixin::is_impl<MyTemplate<int>, template_name_mixin>,
              "MyTemplate<int> has template_name_mixin");
static_assert(xcmixin::is_impl<MyTemplate<float>, template_name_mixin>,
              "MyTemplate<float> has template_name_mixin");
static_assert(xcmixin::is_impl<MixedTemplate<int>, static_mixin>,
              "MixedTemplate<int> has static_mixin");

int main() {
    MyTemplate<int> obj_int;
    MyTemplate<float> obj_float;
    MixedTemplate<int> mixed_obj;

    // Verify values
    if (obj_int.get_name() != "template_int") {
        std::cerr << "Failed: MyTemplate<int>::get_name()" << std::endl;
        return 1;
    }
    if (obj_float.get_name() != "template_float") {
        std::cerr << "Failed: MyTemplate<float>::get_name()" << std::endl;
        return 2;
    }
    if (mixed_obj.get_static_value() != 99) {
        std::cerr << "Failed: MixedTemplate<int>::get_static_value()" << std::endl;
        return 3;
    }

    std::cout << "Template mixin test passed!" << std::endl;
    return 0;
}
