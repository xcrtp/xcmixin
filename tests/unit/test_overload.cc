// Unit tests for overload resolution
#include <type_traits>
#include "xcmixin/xcmixin.hpp"

// Test: overload type is available
using overload_test = xcmixin::overload<int(int)>;
static_assert(sizeof(overload_test) > 0, "overload type exists");

// Test: ret type
using ret_int = xcmixin::ret<int>;
static_assert(std::is_same_v<ret_int, xcmixin::details::ret<int>>,
              "ret type works");

// Test: overload::overloader type is available for basic type
using overloader_test = xcmixin::overload<int(int)>::overloader<void>;
static_assert(sizeof(overloader_test) > 0, "overloader type exists");

int main() {
    // All tests use static_assert, so if we reach here, all tests passed
    return 0;
}
