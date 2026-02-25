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
// ============================================================================
XCMIXIN_DEF_BEGIN(component_info_mixin)
std::string component_count() const { return "3"; }
std::string component_names() const { return "x, y, z"; }
XCMIXIN_DEF_END()

// ============================================================================
// Mixin: Vector statistics (returns float, not Vec3)
// ============================================================================
XCMIXIN_DEF_BEGIN(stats_mixin)
float sum() const {
    return xcmixin_const_self[0] + xcmixin_const_self[1] +
           xcmixin_const_self[2];
}

float product() const {
    return xcmixin_const_self[0] * xcmixin_const_self[1] *
           xcmixin_const_self[2];
}

float max_component() const {
    float m = xcmixin_const_self[0];
    if (xcmixin_const_self[1] > m) m = xcmixin_const_self[1];
    if (xcmixin_const_self[2] > m) m = xcmixin_const_self[2];
    return m;
}

float min_component() const {
    float m = xcmixin_const_self[0];
    if (xcmixin_const_self[1] < m) m = xcmixin_const_self[1];
    if (xcmixin_const_self[2] < m) m = xcmixin_const_self[2];
    return m;
}

float length() const {
    float x = xcmixin_const_self[0];
    float y = xcmixin_const_self[1];
    float z = xcmixin_const_self[2];
    return std::sqrt(x * x + y * y + z * z);
}
XCMIXIN_DEF_END()

// ============================================================================
// Mixin: Print functionality (returns void)
// ============================================================================
XCMIXIN_DEF_BEGIN(print_mixin)
void print() const {
    std::cout << "vec3(" << xcmixin_const_self[0] << ", "
              << xcmixin_const_self[1] << ", " << xcmixin_const_self[2] << ")";
}

void println() const {
    xcmixin_const_self.print();
    std::cout << std::endl;
}
XCMIXIN_DEF_END()

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
// Implement mixins
// ============================================================================
XCMIXIN_IMPL_BEGIN(component_info_mixin)
XCMIXIN_IMPL_FOR(Vec3)
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(stats_mixin)
XCMIXIN_IMPL_FOR(Vec3)
XCMIXIN_IMPL_END()

XCMIXIN_IMPL_BEGIN(print_mixin)
XCMIXIN_IMPL_FOR(Vec3)
XCMIXIN_IMPL_END()

// ============================================================================
// Free functions for vector operations (not from mixins)
// ============================================================================
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
    float len = a.length();  // Use mixin method
    return len > 0.0f ? vector_scale(a, 1.0f / len) : Vec3{};
}

Vec3 vector_lerp(const Vec3& a, const Vec3& b, float t) {
    return vector_add(vector_scale(a, 1.0f - t), vector_scale(b, t));
}

// Custom stream operator (not from mixin, but defined outside)
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

    // Mixin methods
    std::cout << "Mixin methods:" << std::endl;
    std::cout << "  a.component_count() = " << a.component_count() << std::endl;
    std::cout << "  a.component_names() = " << a.component_names() << std::endl;
    std::cout << "  a.sum() = " << a.sum() << std::endl;
    std::cout << "  a.product() = " << a.product() << std::endl;
    std::cout << "  a.max_component() = " << a.max_component() << std::endl;
    std::cout << "  a.min_component() = " << a.min_component() << std::endl;
    std::cout << "  a.length() = " << a.length() << std::endl;
    std::cout << "  a.print(): ";
    a.println();
    std::cout << std::endl;

    // Free functions
    std::cout << "Free functions:" << std::endl;
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
        << "Note: Mixin methods cannot return the class type (Vec3) because"
        << std::endl;
    std::cout
        << "xcmixin requires mixins to be defined before the class uses them."
        << std::endl;
    std::cout << "Vector operations (+, -, *, /) are provided as free "
                 "functions instead."
              << std::endl;
    std::cout << std::endl;
    std::cout << "Create different vector types by composing different mixins!"
              << std::endl;

    return 0;
}
