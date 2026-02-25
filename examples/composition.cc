#include <iostream>
#include <string>
#include <type_traits>

#include "xcmixin/xcmixin.hpp"

// Flexible composition example
// This example demonstrates:
// - Selecting different mixin combinations at compile-time
// - Using mixin_recorder for composition
// - Conditionally enabling/disabling mixins

// ============================================================
// Base service mixins
// ============================================================

class ServiceA;
XCMIXIN_IMPL_AVAILABLE(ServiceA);
XCMIXIN_PRE_DECL(service_a_method)

XCMIXIN_DEF_BEGIN(service_a_method)
void do_a() { std::cout << "ServiceA::do_a()" << std::endl; }
XCMIXIN_DEF_END()

class ServiceB;
XCMIXIN_IMPL_AVAILABLE(ServiceB);
XCMIXIN_PRE_DECL(service_b_method)

XCMIXIN_DEF_BEGIN(service_b_method)
void do_b() { std::cout << "ServiceB::do_b()" << std::endl; }
XCMIXIN_DEF_END()

class ServiceC;
XCMIXIN_IMPL_AVAILABLE(ServiceC);
XCMIXIN_PRE_DECL(service_c_method)

XCMIXIN_DEF_BEGIN(service_c_method)
void do_c() { std::cout << "ServiceC::do_c()" << std::endl; }
XCMIXIN_DEF_END()

// ============================================================
// Service implementations
// ============================================================

using recorder_a = xcmixin::mixin_recorder<service_a_method>;
using recorder_ab = xcmixin::mixin_recorder<service_a_method, service_b_method>;
using recorder_abc = xcmixin::mixin_recorder<service_a_method, service_b_method,
                                             service_c_method>;
using recorder_bc = xcmixin::mixin_recorder<service_b_method, service_c_method>;

class ServiceA : public xcmixin::impl_recorder<ServiceA, recorder_a> {
    xcmixin_init_class;
};
XCMIXIN_IMPL_BEGIN(service_a_method)
XCMIXIN_IMPL_FOR(ServiceA)
void do_a() { std::cout << "ServiceA::do_a()" << std::endl; }
XCMIXIN_IMPL_END()

class ServiceB : public xcmixin::impl_recorder<ServiceB, recorder_a> {
    xcmixin_init_class;
};
XCMIXIN_IMPL_BEGIN(service_a_method)
XCMIXIN_IMPL_FOR(ServiceB)
void do_a() { std::cout << "ServiceB::do_a()" << std::endl; }
XCMIXIN_IMPL_END()

class ServiceC : public xcmixin::impl_recorder<ServiceC, recorder_a> {
    xcmixin_init_class;
};
XCMIXIN_IMPL_BEGIN(service_a_method)
XCMIXIN_IMPL_FOR(ServiceC)
void do_a() { std::cout << "ServiceC::do_a()" << std::endl; }
XCMIXIN_IMPL_END()

// ============================================================
// Dynamic composition using different recorders
// ============================================================

class CompositeService;
XCMIXIN_IMPL_AVAILABLE(CompositeService);

// Declare methods that can be conditionally included
XCMIXIN_PRE_DECL(composite_do_all_method)
XCMIXIN_PRE_DECL(composite_reset_method)
XCMIXIN_PRE_DECL(composite_log_method)

XCMIXIN_DEF_BEGIN(composite_do_all_method)
void do_all() {
    xcmixin_self.do_a();
    xcmixin_self.do_b();
    xcmixin_self.do_c();
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(composite_reset_method)
void reset() { std::cout << "CompositeService::reset()" << std::endl; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(composite_log_method)
void log(const std::string& msg) { std::cout << "[LOG] " << msg << std::endl; }
XCMIXIN_DEF_END()

// Different recorder configurations for different use cases
using recorder_basic =
    xcmixin::mixin_recorder<service_a_method, service_b_method,
                            service_c_method>;
using recorder_with_all =
    xcmixin::mixin_recorder<service_a_method, service_b_method,
                            service_c_method, composite_do_all_method>;
using recorder_with_reset =
    xcmixin::mixin_recorder<service_a_method, service_b_method,
                            service_c_method, composite_do_all_method,
                            composite_reset_method>;
using recorder_full =
    xcmixin::mixin_recorder<service_a_method, service_b_method,
                            service_c_method, composite_do_all_method,
                            composite_reset_method, composite_log_method>;

class CompositeService
    : public xcmixin::impl_recorder<CompositeService, recorder_full> {
    xcmixin_init_class;
};

XCMIXIN_IMPL_BEGIN(service_a_method)
XCMIXIN_IMPL_FOR(CompositeService)
void do_a() { std::cout << "  -> CompositeService::do_a()" << std::endl; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(service_b_method)
XCMIXIN_IMPL_FOR(CompositeService)
void do_b() { std::cout << "  -> CompositeService::do_b()" << std::endl; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(service_c_method)
XCMIXIN_IMPL_FOR(CompositeService)
void do_c() { std::cout << "  -> CompositeService::do_c()" << std::endl; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(composite_do_all_method)
XCMIXIN_IMPL_FOR(CompositeService)
void do_all() {
    std::cout << "CompositeService::do_all():" << std::endl;
    xcmixin_self.do_a();
    xcmixin_self.do_b();
    xcmixin_self.do_c();
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(composite_reset_method)
XCMIXIN_IMPL_FOR(CompositeService)
void reset() { std::cout << "CompositeService::reset()" << std::endl; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(composite_log_method)
XCMIXIN_IMPL_FOR(CompositeService)
void log(const std::string& msg) { std::cout << "[LOG] " << msg << std::endl; }
XCMIXIN_IMPL_END()

// ============================================================
// Separate service classes to demonstrate composition flexibility
// ============================================================

// Minimal service - only A
class MinimalService;
XCMIXIN_IMPL_AVAILABLE(MinimalService);

class MinimalService
    : public xcmixin::impl_recorder<MinimalService, recorder_a> {
    xcmixin_init_class;
};
XCMIXIN_IMPL_BEGIN(service_a_method)
XCMIXIN_IMPL_FOR(MinimalService)
void do_a() { std::cout << "  -> MinimalService::do_a()" << std::endl; }
XCMIXIN_IMPL_END()

// Standard service - A and B
class StandardService;
XCMIXIN_IMPL_AVAILABLE(StandardService);

class StandardService
    : public xcmixin::impl_recorder<StandardService, recorder_ab> {
    xcmixin_init_class;
};
XCMIXIN_IMPL_BEGIN(service_a_method)
XCMIXIN_IMPL_FOR(StandardService)
void do_a() { std::cout << "  -> StandardService::do_a()" << std::endl; }
XCMIXIN_IMPL_END()
XCMIXIN_IMPL_BEGIN(service_b_method)
XCMIXIN_IMPL_FOR(StandardService)
void do_b() { std::cout << "  -> StandardService::do_b()" << std::endl; }
XCMIXIN_IMPL_END()

// Extended service - B and C only
class ExtendedService;
XCMIXIN_IMPL_AVAILABLE(ExtendedService);

class ExtendedService
    : public xcmixin::impl_recorder<ExtendedService, recorder_bc> {
    xcmixin_init_class;
};
XCMIXIN_IMPL_BEGIN(service_b_method)
XCMIXIN_IMPL_FOR(ExtendedService)
void do_b() { std::cout << "  -> ExtendedService::do_b()" << std::endl; }
XCMIXIN_IMPL_END()
XCMIXIN_IMPL_BEGIN(service_c_method)
XCMIXIN_IMPL_FOR(ExtendedService)
void do_c() { std::cout << "  -> ExtendedService::do_c()" << std::endl; }
XCMIXIN_IMPL_END()

// ============================================================
// Runtime mixin selection using function overloading
// ============================================================

class DynamicService;
XCMIXIN_IMPL_AVAILABLE(DynamicService);

XCMIXIN_PRE_DECL(dynamic_process_method)
XCMIXIN_PRE_DECL(dynamic_compute_method)

XCMIXIN_DEF_BEGIN(dynamic_process_method)
void process() { std::cout << "DynamicService::process()" << std::endl; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(dynamic_compute_method)
void compute() { std::cout << "DynamicService::compute()" << std::endl; }
XCMIXIN_DEF_END()

// Recorder for minimal version
using recorder_minimal = xcmixin::mixin_recorder<dynamic_process_method>;
// Recorder for standard version
using recorder_standard =
    xcmixin::mixin_recorder<dynamic_process_method, dynamic_compute_method>;

class DynamicService
    : public xcmixin::impl_recorder<DynamicService, recorder_standard> {
    xcmixin_init_class;
};

XCMIXIN_IMPL_BEGIN(dynamic_process_method)
XCMIXIN_IMPL_FOR(DynamicService)
void process() { std::cout << "DynamicService::process()" << std::endl; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(dynamic_compute_method)
XCMIXIN_IMPL_FOR(DynamicService)
void compute() { std::cout << "DynamicService::compute()" << std::endl; }
XCMIXIN_IMPL_END()

// Function to create service with different capabilities
template <typename Recorder>
xcmixin::impl_recorder<DynamicService, Recorder> create_service() {
    return xcmixin::impl_recorder<DynamicService, Recorder>{};
}

int main() {
    std::cout << "=== Flexible Composition Example ===" << std::endl;

    // ============================================================
    // Example 1: Different predefined recorders
    // ============================================================
    std::cout << "\n--- Example 1: Different Predefined Recorders ---"
              << std::endl;

    // Basic service
    ServiceA svc_a;
    svc_a.do_a();

    // Service with A and B
    ServiceB svc_ab;
    svc_ab.do_a();

    // Full service
    CompositeService full_svc;
    full_svc.do_a();
    full_svc.do_b();
    full_svc.do_c();
    full_svc.do_all();
    full_svc.reset();
    full_svc.log("Hello from composite service");

    // ============================================================
    // Example 2: Different service configurations
    // ============================================================
    std::cout << "\n--- Example 2: Different Service Configurations ---"
              << std::endl;

    // Minimal service
    MinimalService minimal;
    minimal.do_a();

    // Standard service
    StandardService standard;
    standard.do_a();
    standard.do_b();

    // Extended service
    ExtendedService extended;
    extended.do_b();
    extended.do_c();

    // ============================================================
    // Example 3: Check mixin presence at compile time
    // ============================================================
    std::cout << "\n--- Example 3: Compile-time Mixin Checking ---"
              << std::endl;

    // Check if has_mixin
    using test_recorder =
        xcmixin::mixin_recorder<service_a_method, service_b_method>;

    constexpr bool has_a = xcmixin::has_mixin<service_a_method, test_recorder>;
    constexpr bool has_c = xcmixin::has_mixin<service_c_method, test_recorder>;

    std::cout << "has_mixin<service_a_method, recorder>: " << std::boolalpha
              << has_a << std::endl;
    std::cout << "has_mixin<service_c_method, recorder>: " << has_c
              << std::endl;

    // Check if is_impl
    constexpr bool impl_a =
        xcmixin::is_impl<CompositeService, service_a_method>;
    constexpr bool impl_b =
        xcmixin::is_impl<CompositeService, service_b_method>;
    constexpr bool impl_all =
        xcmixin::is_impl<CompositeService, service_a_method, service_b_method,
                         service_c_method>;

    std::cout << "is_impl<CompositeService, service_a_method>: " << impl_a
              << std::endl;
    std::cout << "is_impl<CompositeService, service_b_method>: " << impl_b
              << std::endl;
    std::cout << "is_impl<CompositeService, service_a, service_b, service_c>: "
              << impl_all << std::endl;

    // ============================================================
    // Example 4: Compile-time recorder information
    // ============================================================
    std::cout << "\n--- Example 4: Recorder Type Aliases ---" << std::endl;

    // Each recorder is a distinct type that can be used for different purposes
    std::cout
        << "Created recorder_a, recorder_ab, recorder_abc, and recorder_full"
        << std::endl;
    std::cout << "These can be used to compose different service configurations"
              << std::endl;

    std::cout << "\nAll composition tests completed!" << std::endl;
    return 0;
}
