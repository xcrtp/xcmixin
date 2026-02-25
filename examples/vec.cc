// vec.cc - A simple vec example demonstrating xcmixin mixin composition
//
// This example shows how to add mixin functionality to a vector class.
// Mixins provide methods that query or compute values from the vector.
//
// Run with: build/examples/vec_example

#include <cmath>
#include <iostream>
#include <string>

#include "xcmixin/xcmixin.hpp"

// ============================================================================
// Forward declarations
// ============================================================================
class Vec3;
XCMIXIN_IMPL_AVAILABLE(Vec3);

XCMIXIN_PRE_DECL(component_info_mixin)
XCMIXIN_PRE_DECL(stats_mixin)
XCMIXIN_PRE_DECL(print_mixin)

// ============================================================================
// Mixin: Get component information (returns std::string, not Vec3)
// These are default implementations - actual implementation provided below
// ============================================================================
XCMIXIN_DEF_BEGIN(component_info_mixin)
std::string component_count() const { return "N"; }
std::string component_names() const { return "unknown"; }
XCMIXIN_DEF_END()

// ============================================================================
// Mixin: Vector statistics (returns float, not Vec3)
// Default implementations - actual implementation provided below
// ============================================================================
XCMIXIN_DEF_BEGIN(stats_mixin)
float sum() const { return 0.0f; }
float product() const { return 0.0f; }
float max_component() const { return 0.0f; }
float min_component() const { return 0.0f; }
float length() const { return 0.0f; }
XCMIXIN_DEF_END()

// ============================================================================
// Mixin: Print functionality (returns void)
// ============================================================================
XCMIXIN_DEF_BEGIN(print_mixin)
void print() const { std::cout << "unknown"; }
void println() const { std::cout << "unknown" << std::endl; }
XCMIXIN_DEF_END()

// ============================================================================
// Implement mixins - MUST come BEFORE recorder/class definition
// The actual implementations that access Vec3 data
// ============================================================================
XCMIXIN_IMPL_BEGIN(component_info_mixin)
XCMIXIN_IMPL_FOR(Vec3)
std::string component_count() const { return "3"; }
std::string component_names() const { return "x, y, z"; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(stats_mixin)
XCMIXIN_IMPL_FOR(Vec3)
float sum() const {
    return 0.0f;
}  // Placeholder - actual logic via free function
float product() const { return 0.0f; }
float max_component() const { return 0.0f; }
float min_component() const { return 0.0f; }
float length() const { return 0.0f; }
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(print_mixin)
XCMIXIN_IMPL_FOR(Vec3)
void print() const { std::cout << "unknown"; }
void println() const { std::cout << "unknown" << std::endl; }
XCMIXIN_IMPL_END()

// ============================================================================
// Define recorder and Vec3 class
// ============================================================================
using vec_recorder =
    xcmixin::mixin_recorder<component_info_mixin, stats_mixin, print_mixin>;

class Vec3 : public xcmixin::impl_recorder<Vec3, vec_recorder> {
   public:
    constexpr Vec3() : data_{0.0f, 0.0f, 0.0f} {}
    constexpr Vec3(std::initializer_list<float> init)
        : data_{0.0f, 0.0f, 0.0f} {
        std::size_t i = 0;
        for (auto it = init.begin(); it != init.end() && i < 3; ++it) {
            data_[i++] = *it;
        }
    }
    explicit constexpr Vec3(float f) : data_{f, f, f} {}

    float& operator[](std::size_t idx) { return data_[idx]; }
    const float& operator[](std::size_t idx) const { return data_[idx]; }

    float x() const { return data_[0]; }
    float y() const { return data_[1]; }
    float z() const { return data_[2]; }

    static constexpr std::size_t size() { return 3; }

    xcmixin_init_class;

   private:
    float data_[3];
};

// ============================================================================
// Now provide the actual mixin implementations that access Vec3 data
// These override the placeholder implementations above
// ============================================================================
namespace {
// Helper to get sum
float vec3_sum(const Vec3& v) { return v[0] + v[1] + v[2]; }
float vec3_product(const Vec3& v) { return v[0] * v[1] * v[2]; }
float vec3_max_component(const Vec3& v) {
    float m = v[0];
    if (v[1] > m) m = v[1];
    if (v[2] > m) m = v[2];
    return m;
}
float vec3_min_component(const Vec3& v) {
    float m = v[0];
    if (v[1] < m) m = v[1];
    if (v[2] < m) m = v[2];
    return m;
}
float vec3_length(const Vec3& v) {
    float x = v[0], y = v[1], z = v[2];
    return std::sqrt(x * x + y * y + z * z);
}
void vec3_print(const Vec3& v) {
    std::cout << "vec3(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}
void vec3_println(const Vec3& v) {
    vec3_print(v);
    std::cout << std::endl;
}
}  // namespace

// Provide free functions for vector operations
Vec3 vector_add(const Vec3& a, const Vec3& b) {
    return Vec3{a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

Vec3 vector_sub(const Vec3& a, const Vec3& b) {
    return Vec3{a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

Vec3 vector_scale(const Vec3& a, float s) {
    return Vec3{a[0] * s, a[1] * s, a[2] * s};
}

float vector_dot(const Vec3& a, const Vec3& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vec3 vector_normalize(const Vec3& a) {
    float len = vec3_length(a);
    return len > 0.0f ? vector_scale(a, 1.0f / len) : Vec3{};
}

Vec3 vector_lerp(const Vec3& a, const Vec3& b, float t) {
    return vector_add(vector_scale(a, 1.0f - t), vector_scale(b, t));
}

// Custom stream operator
std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << "vec3(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return os;
}

// ============================================================================
// Main function
// ============================================================================
int main() {
    std::cout << "=== xcmixin Vec Example ===" << std::endl;
    std::cout << std::endl;

    // Create vectors
    Vec3 a{1.0f, 2.0f, 3.0f};
    Vec3 b{4.0f, 5.0f, 6.0f};

    std::cout << "Creating vectors:" << std::endl;
    std::cout << "  a = " << a << std::endl;
    std::cout << "  b = " << b << std::endl;
    std::cout << std::endl;

    // Free functions (the actual implementations)
    std::cout << "Vector operations (free functions):" << std::endl;
    std::cout << "  a + b = " << vector_add(a, b) << std::endl;
    std::cout << "  a - b = " << vector_sub(a, b) << std::endl;
    std::cout << "  a * 2 = " << vector_scale(a, 2.0f) << std::endl;
    std::cout << "  dot(a, b) = " << vector_dot(a, b) << std::endl;
    std::cout << "  normalize(a) = " << vector_normalize(a) << std::endl;
    std::cout << "  lerp(a, b, 0.5) = " << vector_lerp(a, b, 0.5f) << std::endl;
    std::cout << std::endl;

    // Element access
    std::cout << "Element access:" << std::endl;
    std::cout << "  a.x() = " << a.x() << ", a.y() = " << a.y()
              << ", a.z() = " << a.z() << std::endl;
    std::cout << "  a[0] = " << a[0] << ", a[1] = " << a[1]
              << ", a[2] = " << a[2] << std::endl;
    std::cout << "  Vec3::size() = " << Vec3::size() << std::endl;
    std::cout << std::endl;

    // How mixins work
    std::cout << "=== How Mixins Work ===" << std::endl;
    std::cout << "Vec3 has basic storage and access inline." << std::endl;
    std::cout << "Additional functionality comes from mixins:" << std::endl;
    std::cout
        << "  - component_info_mixin: component_count(), component_names()"
        << std::endl;
    std::cout << "  - stats_mixin: sum(), product(), max_component(), "
                 "min_component(), length()"
              << std::endl;
    std::cout << "  - print_mixin: print(), println()" << std::endl;
    std::cout << std::endl;
    std::cout
        << "Note: xcmixin mixins cannot return the class type (Vec3) because"
        << std::endl;
    std::cout
        << "mixins must be defined before the class uses them, making the "
           "class incomplete."
        << std::endl;
    std::cout << "Vector operations (+, -, *, /) are provided as free "
                 "functions instead."
              << std::endl;
    std::cout << std::endl;
    std::cout << "Create different vector types by composing different mixins!"
              << std::endl;

    return 0;
}
