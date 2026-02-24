cmake_minimum_required(VERSION 3.10)
option(XCMIXIN_BUILD_TESTS "Build xcmixin tests" ON)

if(NOT XCMIXIN_BUILD_TESTS)
    return()
endif()

# Simple test runner - compiles test files and runs them
# Tests rely on compile-time static_assert for validation

# Unit tests
add_executable(test_mixin_recorder unit/test_mixin_recorder.cc)
target_link_libraries(test_mixin_recorder PRIVATE xcmixin)
add_test(NAME MixinRecorderTest COMMAND test_mixin_recorder)

add_executable(test_impl_mixin unit/test_impl_mixin.cc)
target_link_libraries(test_impl_mixin PRIVATE xcmixin)
add_test(NAME ImplMixinTest COMMAND test_impl_mixin)

add_executable(test_overload unit/test_overload.cc)
target_link_libraries(test_overload PRIVATE xcmixin)
add_test(NAME OverloadTest COMMAND test_overload)

add_executable(test_macros unit/test_macros.cc)
target_link_libraries(test_macros PRIVATE xcmixin)
add_test(NAME MacrosTest COMMAND test_macros)

# Integration tests
add_executable(test_composition integration/test_composition.cc)
target_link_libraries(test_composition PRIVATE xcmixin)
add_test(NAME CompositionTest COMMAND test_composition)

add_executable(test_template_mixin integration/test_template_mixin.cc)
target_link_libraries(test_template_mixin PRIVATE xcmixin)
add_test(NAME TemplateMixinTest COMMAND test_template_mixin)

add_executable(test_method_hiding integration/test_method_hiding.cc)
target_link_libraries(test_method_hiding PRIVATE xcmixin)
add_test(NAME MethodHidingTest COMMAND test_method_hiding)
