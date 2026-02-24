// Unit tests for mixin_recorder type
#include <type_traits>
#include "xcmixin/xcmixin.hpp"

// Test mixin declarations
XCMIXIN_PRE_DECL(mixin_a)
XCMIXIN_PRE_DECL(mixin_b)
XCMIXIN_PRE_DECL(mixin_c)

// Mixin definitions
XCMIXIN_DEF_BEGIN(mixin_a)
int value_a() { return 1; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(mixin_b)
int value_b() { return 2; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(mixin_c)
int value_c() { return 3; }
XCMIXIN_DEF_END()

// Test: mixin_recorder stores types correctly
static_assert(
    std::is_same_v<xcmixin::mixin_recorder<mixin_a>,
                   xcmixin::mixin_recorder<mixin_a>>,
    "mixin_recorder with single mixin");

// Test: push_back adds mixin to end
using recorder_ab = xcmixin::mixin_recorder<mixin_a>::push_back<mixin_b>;
static_assert(
    std::is_same_v<recorder_ab, xcmixin::mixin_recorder<mixin_a, mixin_b>>,
    "push_back adds mixin to end");

// Test: push_front adds mixin to beginning
using recorder_ba = xcmixin::mixin_recorder<mixin_b>::push_front<mixin_a>;
static_assert(
    std::is_same_v<recorder_ba, xcmixin::mixin_recorder<mixin_a, mixin_b>>,
    "push_front adds mixin to beginning");

// Test: concat combines two recorders
using recorder_abc =
    xcmixin::mixin_recorder<mixin_a>::concat<xcmixin::mixin_recorder<mixin_b, mixin_c>>;
static_assert(
    std::is_same_v<recorder_abc, xcmixin::mixin_recorder<mixin_a, mixin_b, mixin_c>>,
    "concat combines recorders");

// Test: has_mixin checks if mixin is in recorder
static_assert(xcmixin::has_mixin<mixin_a, xcmixin::mixin_recorder<mixin_a, mixin_b>>,
              "has_mixin finds first mixin");
static_assert(xcmixin::has_mixin<mixin_b, xcmixin::mixin_recorder<mixin_a, mixin_b>>,
              "has_mixin finds second mixin");
static_assert(!xcmixin::has_mixin<mixin_c, xcmixin::mixin_recorder<mixin_a, mixin_b>>,
              "has_mixin returns false for missing mixin");

// Test: recorder_concat combines multiple recorders
using concat_result =
    xcmixin::recorder_concat<xcmixin::mixin_recorder<mixin_a>,
                             xcmixin::mixin_recorder<mixin_b, mixin_c>>;
static_assert(
    std::is_same_v<concat_result, xcmixin::mixin_recorder<mixin_a, mixin_b, mixin_c>>,
    "recorder_concat combines multiple recorders");

int main() {
    // All tests use static_assert, so if we reach here, all tests passed
    return 0;
}
