#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "xcmixin/xcmixin.hpp"

// Decorator pattern using xcmixin
// This example demonstrates:
// - Logging decorator: logs method calls
// - Timing decorator: measures execution time
// - Validation decorator: validates input parameters
// - Decorator composition: chaining multiple decorators

class Calculator;
XCMIXIN_IMPL_AVAILABLE(Calculator);

XCMIXIN_PRE_DECL(calc_add_method)
XCMIXIN_PRE_DECL(calc_sub_method)
XCMIXIN_PRE_DECL(calc_mul_method)

// Base calculator interface
XCMIXIN_DEF_BEGIN(calc_add_method)
int add(int a, int b) { return a + b; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(calc_sub_method)
int sub(int a, int b) { return a - b; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(calc_mul_method)
int mul(int a, int b) { return a * b; }
XCMIXIN_DEF_END()

// Basic calculator recorder
using calc_recorder =
    xcmixin::mixin_recorder<calc_add_method, calc_sub_method, calc_mul_method>;

// Implementation for base calculator
class Calculator : public xcmixin::impl_recorder<Calculator, calc_recorder> {
    xcmixin_init_class;
};

XCMIXIN_IMPL_BEGIN(calc_add_method)
XCMIXIN_IMPL_FOR(Calculator)
int add(int a, int b) { return a + b; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(calc_sub_method)
XCMIXIN_IMPL_FOR(Calculator)
int sub(int a, int b) { return a - b; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(calc_mul_method)
XCMIXIN_IMPL_FOR(Calculator)
int mul(int a, int b) { return a * b; }
XCMIXIN_IMPL_END()

// ============================================================
// Decorator 1: Logging Decorator
// ============================================================
class LoggingCalculator;
XCMIXIN_IMPL_AVAILABLE(LoggingCalculator);

XCMIXIN_PRE_DECL(logging_add_method)
XCMIXIN_PRE_DECL(logging_sub_method)
XCMIXIN_PRE_DECL(logging_mul_method)

XCMIXIN_DEF_BEGIN(logging_add_method)
int add(int a, int b) {
    std::cout << "[LOG] add(" << a << ", " << b << ")" << std::endl;
    int result = xcmixin_self.get_impl().add(a, b);
    std::cout << "[LOG] add result: " << result << std::endl;
    return result;
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(logging_sub_method)
int sub(int a, int b) {
    std::cout << "[LOG] sub(" << a << ", " << b << ")" << std::endl;
    int result = xcmixin_self.get_impl().sub(a, b);
    std::cout << "[LOG] sub result: " << result << std::endl;
    return result;
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(logging_mul_method)
int mul(int a, int b) {
    std::cout << "[LOG] mul(" << a << ", " << b << ")" << std::endl;
    int result = xcmixin_self.get_impl().mul(a, b);
    std::cout << "[LOG] mul result: " << result << std::endl;
    return result;
}
XCMIXIN_DEF_END()

using logging_recorder =
    xcmixin::mixin_recorder<logging_add_method, logging_sub_method,
                            logging_mul_method>;

class LoggingCalculator
    : public xcmixin::impl_recorder<LoggingCalculator, logging_recorder> {
   public:
    LoggingCalculator(Calculator& calc) : calc_(calc) {}

    Calculator& get_impl() { return calc_; }

   private:
    Calculator& calc_;
};

XCMIXIN_IMPL_BEGIN(logging_add_method)
XCMIXIN_IMPL_FOR(LoggingCalculator)
int add(int a, int b) {
    std::cout << "[LOG] add(" << a << ", " << b << ")" << std::endl;
    int result = xcmixin_self.get_impl().add(a, b);
    std::cout << "[LOG] add result: " << result << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(logging_sub_method)
XCMIXIN_IMPL_FOR(LoggingCalculator)
int sub(int a, int b) {
    std::cout << "[LOG] sub(" << a << ", " << b << ")" << std::endl;
    int result = xcmixin_self.get_impl().sub(a, b);
    std::cout << "[LOG] sub result: " << result << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(logging_mul_method)
XCMIXIN_IMPL_FOR(LoggingCalculator)
int mul(int a, int b) {
    std::cout << "[LOG] mul(" << a << ", " << b << ")" << std::endl;
    int result = xcmixin_self.get_impl().mul(a, b);
    std::cout << "[LOG] mul result: " << result << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

// ============================================================
// Decorator 2: Timing Decorator
// ============================================================
class TimingCalculator;
XCMIXIN_IMPL_AVAILABLE(TimingCalculator);

XCMIXIN_PRE_DECL(timing_add_method)
XCMIXIN_PRE_DECL(timing_sub_method)
XCMIXIN_PRE_DECL(timing_mul_method)

XCMIXIN_DEF_BEGIN(timing_add_method)
int add(int a, int b) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().add(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] add took: " << duration.count() << " us"
              << std::endl;
    return result;
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(timing_sub_method)
int sub(int a, int b) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().sub(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] sub took: " << duration.count() << " us"
              << std::endl;
    return result;
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(timing_mul_method)
int mul(int a, int b) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().mul(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] mul took: " << duration.count() << " us"
              << std::endl;
    return result;
}
XCMIXIN_DEF_END()

using timing_recorder =
    xcmixin::mixin_recorder<timing_add_method, timing_sub_method,
                            timing_mul_method>;

class TimingCalculator
    : public xcmixin::impl_recorder<TimingCalculator, timing_recorder> {
   public:
    TimingCalculator(Calculator& calc) : calc_(calc) {}

    Calculator& get_impl() { return calc_; }

   private:
    Calculator& calc_;
};

XCMIXIN_IMPL_BEGIN(timing_add_method)
XCMIXIN_IMPL_FOR(TimingCalculator)
int add(int a, int b) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().add(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] add took: " << duration.count() << " us"
              << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(timing_sub_method)
XCMIXIN_IMPL_FOR(TimingCalculator)
int sub(int a, int b) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().sub(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] sub took: " << duration.count() << " us"
              << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(timing_mul_method)
XCMIXIN_IMPL_FOR(TimingCalculator)
int mul(int a, int b) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().mul(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] mul took: " << duration.count() << " us"
              << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

// ============================================================
// Decorator 3: Validation Decorator
// ============================================================
class ValidatingCalculator;
XCMIXIN_IMPL_AVAILABLE(ValidatingCalculator);

XCMIXIN_PRE_DECL(validating_add_method)
XCMIXIN_PRE_DECL(validating_sub_method)
XCMIXIN_PRE_DECL(validating_mul_method)

XCMIXIN_DEF_BEGIN(validating_add_method)
int add(int a, int b) {
    if (a < 0 || b < 0) {
        std::cout << "[VALIDATION] Warning: negative numbers not allowed"
                  << std::endl;
    }
    return xcmixin_self.get_impl().add(a, b);
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(validating_sub_method)
int sub(int a, int b) {
    if (a < b) {
        std::cout << "[VALIDATION] Warning: result may be negative"
                  << std::endl;
    }
    return xcmixin_self.get_impl().sub(a, b);
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(validating_mul_method)
int mul(int a, int b) {
    if (a == 0 || b == 0) {
        std::cout << "[VALIDATION] Multiplying by zero" << std::endl;
    }
    return xcmixin_self.get_impl().mul(a, b);
}
XCMIXIN_DEF_END()

using validating_recorder =
    xcmixin::mixin_recorder<validating_add_method, validating_sub_method,
                            validating_mul_method>;

class ValidatingCalculator
    : public xcmixin::impl_recorder<ValidatingCalculator, validating_recorder> {
   public:
    ValidatingCalculator(Calculator& calc) : calc_(calc) {}

    Calculator& get_impl() { return calc_; }

   private:
    Calculator& calc_;
};

XCMIXIN_IMPL_BEGIN(validating_add_method)
XCMIXIN_IMPL_FOR(ValidatingCalculator)
int add(int a, int b) {
    if (a < 0 || b < 0) {
        std::cout << "[VALIDATION] Warning: negative numbers not allowed"
                  << std::endl;
    }
    return xcmixin_self.get_impl().add(a, b);
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(validating_sub_method)
XCMIXIN_IMPL_FOR(ValidatingCalculator)
int sub(int a, int b) {
    if (a < b) {
        std::cout << "[VALIDATION] Warning: result may be negative"
                  << std::endl;
    }
    return xcmixin_self.get_impl().sub(a, b);
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(validating_mul_method)
XCMIXIN_IMPL_FOR(ValidatingCalculator)
int mul(int a, int b) {
    if (a == 0 || b == 0) {
        std::cout << "[VALIDATION] Multiplying by zero" << std::endl;
    }
    return xcmixin_self.get_impl().mul(a, b);
}
XCMIXIN_IMPL_END()

// ============================================================
// Composed Decorator: Logging + Timing + Validation
// ============================================================
class ComposedDecorator;
XCMIXIN_IMPL_AVAILABLE(ComposedDecorator);

XCMIXIN_PRE_DECL(composed_add_method)
XCMIXIN_PRE_DECL(composed_sub_method)
XCMIXIN_PRE_DECL(composed_mul_method)

XCMIXIN_DEF_BEGIN(composed_add_method)
int add(int a, int b) {
    // Validation
    if (a < 0 || b < 0) {
        std::cout << "[VALIDATION] Warning: negative numbers not allowed"
                  << std::endl;
    }
    // Logging
    std::cout << "[LOG] add(" << a << ", " << b << ")" << std::endl;
    // Timing
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().add(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] add took: " << duration.count() << " us"
              << std::endl;
    std::cout << "[LOG] add result: " << result << std::endl;
    return result;
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(composed_sub_method)
int sub(int a, int b) {
    // Validation
    if (a < b) {
        std::cout << "[VALIDATION] Warning: result may be negative"
                  << std::endl;
    }
    // Logging
    std::cout << "[LOG] sub(" << a << ", " << b << ")" << std::endl;
    // Timing
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().sub(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] sub took: " << duration.count() << " us"
              << std::endl;
    std::cout << "[LOG] sub result: " << result << std::endl;
    return result;
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(composed_mul_method)
int mul(int a, int b) {
    // Validation
    if (a == 0 || b == 0) {
        std::cout << "[VALIDATION] Multiplying by zero" << std::endl;
    }
    // Logging
    std::cout << "[LOG] mul(" << a << ", " << b << ")" << std::endl;
    // Timing
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().mul(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] mul took: " << duration.count() << " us"
              << std::endl;
    std::cout << "[LOG] mul result: " << result << std::endl;
    return result;
}
XCMIXIN_DEF_END()

using composed_recorder =
    xcmixin::mixin_recorder<composed_add_method, composed_sub_method,
                            composed_mul_method>;

class ComposedDecorator
    : public xcmixin::impl_recorder<ComposedDecorator, composed_recorder> {
   public:
    ComposedDecorator(Calculator& calc) : calc_(calc) {}

    Calculator& get_impl() { return calc_; }

   private:
    Calculator& calc_;
};

XCMIXIN_IMPL_BEGIN(composed_add_method)
XCMIXIN_IMPL_FOR(ComposedDecorator)
int add(int a, int b) {
    // Validation
    if (a < 0 || b < 0) {
        std::cout << "[VALIDATION] Warning: negative numbers not allowed"
                  << std::endl;
    }
    // Logging
    std::cout << "[LOG] add(" << a << ", " << b << ")" << std::endl;
    // Timing
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().add(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] add took: " << duration.count() << " us"
              << std::endl;
    std::cout << "[LOG] add result: " << result << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(composed_sub_method)
XCMIXIN_IMPL_FOR(ComposedDecorator)
int sub(int a, int b) {
    // Validation
    if (a < b) {
        std::cout << "[VALIDATION] Warning: result may be negative"
                  << std::endl;
    }
    // Logging
    std::cout << "[LOG] sub(" << a << ", " << b << ")" << std::endl;
    // Timing
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().sub(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] sub took: " << duration.count() << " us"
              << std::endl;
    std::cout << "[LOG] sub result: " << result << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(composed_mul_method)
XCMIXIN_IMPL_FOR(ComposedDecorator)
int mul(int a, int b) {
    // Validation
    if (a == 0 || b == 0) {
        std::cout << "[VALIDATION] Multiplying by zero" << std::endl;
    }
    // Logging
    std::cout << "[LOG] mul(" << a << ", " << b << ")" << std::endl;
    // Timing
    auto start = std::chrono::high_resolution_clock::now();
    int result = xcmixin_self.get_impl().mul(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[TIMING] mul took: " << duration.count() << " us"
              << std::endl;
    std::cout << "[LOG] mul result: " << result << std::endl;
    return result;
}
XCMIXIN_IMPL_END()

int main() {
    std::cout << "=== Decorator Pattern Example ===" << std::endl;

    Calculator base_calc;

    std::cout << "\n--- Basic Calculator ---" << std::endl;
    std::cout << "add(10, 5) = " << base_calc.add(10, 5) << std::endl;
    std::cout << "sub(10, 5) = " << base_calc.sub(10, 5) << std::endl;
    std::cout << "mul(10, 5) = " << base_calc.mul(10, 5) << std::endl;

    std::cout << "\n--- Logging Decorator ---" << std::endl;
    LoggingCalculator logged_calc(base_calc);
    std::cout << "add(20, 10) = " << logged_calc.add(20, 10) << std::endl;

    std::cout << "\n--- Timing Decorator ---" << std::endl;
    TimingCalculator timed_calc(base_calc);
    std::cout << "add(30, 15) = " << timed_calc.add(30, 15) << std::endl;

    std::cout << "\n--- Validation Decorator ---" << std::endl;
    ValidatingCalculator validated_calc(base_calc);
    std::cout << "add(-5, 10) = " << validated_calc.add(-5, 10) << std::endl;
    std::cout << "sub(5, 10) = " << validated_calc.sub(5, 10) << std::endl;

    std::cout << "\n--- Composed Decorator (Logging + Timing + Validation) ---"
              << std::endl;
    ComposedDecorator composed_calc(base_calc);
    std::cout << "add(100, 50) = " << composed_calc.add(100, 50) << std::endl;
    std::cout << "sub(100, 50) = " << composed_calc.sub(100, 50) << std::endl;
    std::cout << "mul(100, 50) = " << composed_calc.mul(100, 50) << std::endl;

    std::cout << "\nAll decorator tests completed!" << std::endl;
    return 0;
}
