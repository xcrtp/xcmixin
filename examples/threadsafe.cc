#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "xcmixin/xcmixin.hpp"

// Thread-safe reference wrapper using xcmixin
// This example demonstrates:
// - Using xcmixin to compose thread-safe services
// - Demonstrating mutex patterns with decorators

class Calculator;
XCMIXIN_IMPL_AVAILABLE(Calculator);

XCMIXIN_PRE_DECL(calc_add_method)
XCMIXIN_PRE_DECL(calc_sub_method)
XCMIXIN_PRE_DECL(calc_get_result_method)
XCMIXIN_PRE_DECL(calc_set_result_method)

// Basic calculator operations
XCMIXIN_DEF_BEGIN(calc_add_method)
int add(int a, int b) { return a + b; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(calc_sub_method)
int sub(int a, int b) { return a - b; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(calc_get_result_method)
int get_result() const { return 0; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(calc_set_result_method)
void set_result(int) {}
XCMIXIN_DEF_END()

// Full calculator recorder
using calc_recorder =
    xcmixin::mixin_recorder<calc_add_method, calc_sub_method,
                            calc_get_result_method, calc_set_result_method>;

// Calculator with state
class Calculator : public xcmixin::impl_recorder<Calculator, calc_recorder> {
   public:
    int get_result() const { return result_; }
    void set_result(int value) { result_ = value; }

   private:
    int result_ = 0;
};

XCMIXIN_IMPL_BEGIN(calc_add_method)
XCMIXIN_IMPL_FOR(Calculator)
int add(int a, int b) { return a + b; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(calc_sub_method)
XCMIXIN_IMPL_FOR(Calculator)
int sub(int a, int b) { return a - b; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(calc_get_result_method)
XCMIXIN_IMPL_FOR(Calculator)
int get_result() const { return result_; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(calc_set_result_method)
XCMIXIN_IMPL_FOR(Calculator)
void set_result(int value) { result_ = value; }
XCMIXIN_IMPL_END()

// ============================================================
// Thread-safe calculator with mutex
// ============================================================

class ThreadSafeCalculator;
XCMIXIN_IMPL_AVAILABLE(ThreadSafeCalculator);

XCMIXIN_PRE_DECL(ts_calc_add_method)
XCMIXIN_PRE_DECL(ts_calc_sub_method)
XCMIXIN_PRE_DECL(ts_calc_get_result_method)
XCMIXIN_PRE_DECL(ts_calc_set_result_method)

XCMIXIN_DEF_BEGIN(ts_calc_add_method)
int add(int a, int b) {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    return xcmixin_self.get_impl().add(a, b);
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(ts_calc_sub_method)
int sub(int a, int b) {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    return xcmixin_self.get_impl().sub(a, b);
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(ts_calc_get_result_method)
int get_result() {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    return xcmixin_self.get_impl().get_result();
}
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(ts_calc_set_result_method)
void set_result(int value) {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    xcmixin_self.get_impl().set_result(value);
}
XCMIXIN_DEF_END()

using ts_calc_recorder =
    xcmixin::mixin_recorder<ts_calc_add_method, ts_calc_sub_method,
                            ts_calc_get_result_method,
                            ts_calc_set_result_method>;

class ThreadSafeCalculator
    : public xcmixin::impl_recorder<ThreadSafeCalculator, ts_calc_recorder> {
   public:
    ThreadSafeCalculator(Calculator& impl) : impl_(impl) {}

    Calculator& get_impl() { return impl_; }
    std::mutex& get_mutex() { return mutex_; }

   private:
    Calculator& impl_;
    mutable std::mutex mutex_;
};

XCMIXIN_IMPL_BEGIN(ts_calc_add_method)
XCMIXIN_IMPL_FOR(ThreadSafeCalculator)
int add(int a, int b) {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    return xcmixin_self.get_impl().add(a, b);
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(ts_calc_sub_method)
XCMIXIN_IMPL_FOR(ThreadSafeCalculator)
int sub(int a, int b) {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    return xcmixin_self.get_impl().sub(a, b);
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(ts_calc_get_result_method)
XCMIXIN_IMPL_FOR(ThreadSafeCalculator)
int get_result() {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    return xcmixin_self.get_impl().get_result();
}
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(ts_calc_set_result_method)
XCMIXIN_IMPL_FOR(ThreadSafeCalculator)
void set_result(int value) {
    std::lock_guard<std::mutex> lock(xcmixin_self.get_mutex());
    xcmixin_self.get_impl().set_result(value);
}
XCMIXIN_IMPL_END()

int main() {
    std::cout << "=== Thread-safe Reference Wrapper Example ===" << std::endl;

    // Basic calculator
    Calculator calc;
    calc.set_result(100);
    std::cout << "Basic calc: add(10, 20) = " << calc.add(10, 20) << std::endl;
    std::cout << "Basic calc: get_result() = " << calc.get_result()
              << std::endl;

    std::cout << "\n--- Thread-safe Calculator (mutex decorator) ---"
              << std::endl;

    // Thread-safe calculator
    ThreadSafeCalculator ts_calc(calc);

    // Run multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&ts_calc, i]() {
            for (int j = 0; j < 5; ++j) {
                int result = ts_calc.add(i * 10, j);
                std::cout << "Thread " << i << ": add(" << i * 10 << ", " << j
                          << ") = " << result << std::endl;
                ts_calc.set_result(result);
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final result: " << ts_calc.get_result() << std::endl;

    std::cout << "\n--- Multiple calculators with separate locks ---"
              << std::endl;

    // Create multiple independent calculators
    Calculator calc1;
    Calculator calc2;
    ThreadSafeCalculator ts_calc1(calc1);
    ThreadSafeCalculator ts_calc2(calc2);

    ts_calc1.set_result(0);
    ts_calc2.set_result(100);

    std::vector<std::thread> workers;
    workers.emplace_back([&ts_calc1]() {
        for (int i = 0; i < 10; ++i) {
            ts_calc1.add(i, 1);
        }
    });
    workers.emplace_back([&ts_calc2]() {
        for (int i = 0; i < 10; ++i) {
            ts_calc2.add(i, 1);
        }
    });

    for (auto& t : workers) {
        t.join();
    }

    std::cout << "calc1 result: " << ts_calc1.get_result() << std::endl;
    std::cout << "calc2 result: " << ts_calc2.get_result() << std::endl;

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
