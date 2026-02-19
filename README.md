# xcmixin

[**中文文档**](README_CN.md)

**xcmixin** is a modern C++ static mixin solution that uses CRTP to compose multiple methods into classes at compile time, without modifying the original class definition.

## Quick Start

### Build

```bash
# Configure and build
cmake -B build -G Ninja
cmake --build build

# Run example
build/examples/oop_example
```

### Quick Example

```cpp
#include <iostream>
#include <string>
#include "xcmixin/xcmixin.hpp"

class MyClass;
XCMIXIN_IMPL_AVAILABLE(MyClass);

// 1. Define mixin methods
XCMIXIN_DEF_BEGIN(name_method)
std::string name() { return "Unknown"; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(print_method)
void print() { std::cout << xcmixin_self.name() << std::endl; }
XCMIXIN_DEF_END()

// 2. Implement mixin (optional: provide class-specific implementation)
XCMIXIN_IMPL_BEGIN(name_method)
XCMIXIN_IMPL_FOR(MyClass)
std::string name() { return "MyClass"; }
XCMIXIN_IMPL_END()

// 3. Combine mixins
using recorder = xcmixin::mixin_recorder<print_method, name_method>;

// 4. Inject into class
class MyClass : public xcmixin::impl_recorder<MyClass, recorder> {
    xcmixin_init_class;
};

int main() {
    MyClass obj;
    obj.print();  // Output: MyClass
    obj.name();
    return 0;
}
```

## Safety

Compile-time validation ensures mixin correctness:

**Class Forward Declaration**
```cpp
class MyClass;
XCMIXIN_IMPL_AVAILABLE(MyClass);  // All injection must occur before class definition
// ...
class MyClass { /* ... */ };
```

**Dependency Constraints**
```cpp
XCMIXIN_REQUIRE(print_method,
    xcmixin_require_mixin(name_method););
```

**Method Signature Validation**
```cpp
XCMIXIN_REQUIRE(print_method,
    xcmixin_require_method(print, void);  // Validates const/volatile/const volatile overloads
);
```

**Hiding Detection**
```cpp
XCMIXIN_REQUIRE(name_method,
    xcmixin_no_hiding(name, long, int);  // Validates name(int, long) overload is not hidden
    xcmixin_no_hiding(name, int, const_););
```

## Flexibility

### Default Implementation

Define reusable method collections:

```cpp
XCMIXIN_DEF_BEGIN(new_name_method)
std::string name() { return "NewName"; }
std::string name() const { return "NewName"; }
XCMIXIN_DEF_END()
```

### Cross-Mixin Access

Use `xcmixin_self` / `xcmixin_const_self` to access all methods of the final derived class from any mixin:

```cpp
XCMIXIN_DEF_BEGIN(print_method)
void print() {
    std::cout << xcmixin_self.name() << std::endl;
    std::cout << xcmixin_self.name(11) << std::endl;
    std::cout << xcmixin_const_self.name(11) << std::endl;
}
XCMIXIN_DEF_END()
```

### Specialized Implementation

Provide customized method implementations for specific derived classes:

```cpp
XCMIXIN_IMPL_BEGIN(name_method)
XCMIXIN_IMPL_FOR(MyClass)
std::string name() { return "MyClass"; }
std::string name(int i) { return "MyClass " + std::to_string(i); }
std::string name(int i) const { return "const MyClass " + std::to_string(i); }
std::string name(long i) const { return "const MyClass " + std::to_string(i); }
XCMIXIN_IMPL_END()
```

### Mixin Extension

Derive new mixins from existing ones:

```cpp
XCMIXIN_DEF_BEGIN(name_method)
std::string name() { return "Unknown"; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_EXTEND_BEGIN(new_name_method, name_method)
using base::name;  // `base` refers to the direct base class
std::string name() { return "NewName"; }
XCMIXIN_DEF_END()
```

### Implementation Extension

Extend implemented mixins for specific derived classes:

```cpp
XCMIXIN_IMPL_BEGIN(new_name_method)
XCMIXIN_IMPL_EXTEND_FOR(name_method, MyClass)
using base::name;
std::string name() { return "NewName"; }
XCMIXIN_IMPL_END()
```

### Template Class Support

xcmixin also supports template classes. Use `XCMIXIN_PRE_DECL` for forward declaration and `xcmixin_init_template` for validation:

```cpp
template <typename T>
class MyTemplate;
XCMIXIN_PRE_DECL(name_mixin)
XCMIXIN_REQUIRE(name_mixin, xcmixin_no_hiding(name);)

XCMIXIN_DEF_BEGIN(name_mixin)
std::string name() { return "Unknown"; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(sayhello_mixin)
void say_hello() { std::cout << xcmixin_self.name() << " hello " << std::endl; }
XCMIXIN_DEF_END()

using recorder = xcmixin::mixin_recorder<name_mixin, sayhello_mixin>;

template <typename T>
class MyTemplate : public xcmixin::impl_recorder<MyTemplate<T>, recorder> {
    xcmixin_init_template(xcmixin::impl_recorder<MyTemplate<T>, recorder>);
};
```

`XCMIXIN_IMPL_BEGIN` supports extended arguments to declare template parameters, enabling specialized implementations for different template specializations:

```cpp
// Generic template implementation
XCMIXIN_IMPL_BEGIN(name_mixin, typename T)
XCMIXIN_IMPL_FOR(MyTemplate<T>)
std::string name() { return "MyTemplate default"; }
XCMIXIN_IMPL_END()

// MyTemplate<int> specialization
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<int>)
std::string name() { return "MyTemplate<int>"; }
XCMIXIN_IMPL_END()

// MyTemplate<float> specialization
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<float>)
std::string name() { return "MyTemplate<float>"; }
XCMIXIN_IMPL_END()

int main() {
    MyTemplate<double> obj1;
    obj1.say_hello();  // Output: MyTemplate default hello
    MyTemplate<int> obj2;
    obj2.say_hello();  // Output: MyTemplate<int> hello
    MyTemplate<float> obj3;
    obj3.say_hello();  // Output: MyTemplate<float> hello
}
```

### Combination and Injection

Combine multiple mixins flexibly via `mixin_recorder`:

```cpp
using recorder = xcmixin::mixin_recorder<print_method, new_name_method,
                                          dosomethings1_method>;
```

Inject the combined mixins into the target class:

```cpp
class MyClass : public xcmixin::impl_recorder<MyClass, recorder> {
    xcmixin_init_class;  // Must be called at the end of class definition for compile-time validation
};
```

### Usage

Identical to regular member function calls:

```cpp
MyClass obj;
obj.print();
obj.name();
obj.dosomethings1();
```

### Generic Constraints

Use the `Impl` concept to constrain template types, as an alternative and enhancement to traditional base class references:

```cpp
template <xcmixin::Impl<print_method, name_method> T>
void print(T& p) {
    p.print();
    std::cout << "class_name: " << p.name() << std::endl;
}

int main() {
    MyClass obj;
    print(obj);
    return 0;
}
```

Compared to traditional base class references, the `Impl` concept requires no actual inheritance relationship—just that the derived class includes the specified mixin injection—providing more flexible constraints.

## Zero Overhead

- **Compile-time completion**: All validation occurs at compile time with no runtime overhead
- **Single inheritance chain**: Generates a single-inheritance structure without multiple inheritance or vtable overhead
- **EBO optimization**: Mixins without data members use empty base class optimization, maintaining standard layout

## Use Cases

- **High-performance math libraries**: Provide default implementations for general computations with custom optimizations for specific scenarios
- **Strategy pattern**: Compile-time strategy decisions eliminate runtime dispatch overhead
- **State pattern**: Compile-time state management without runtime state object overhead
- **Interface composition**: Combine different functional modules as needed

## Compatibility

| Compiler | Status |
|----------|--------|
| clang / clang-cl | Full support |
| gcc | Full support |
| MSVC | Partial support |

## Known Issues

### MSVC Overload Resolution Defect

MSVC has an overload resolution defect when processing extended mixin macros. When using `XCMIXIN_DEF_EXTEND_BEGIN` or `XCMIXIN_IMPL_EXTEND_FOR` with `using base::func` to introduce base overload functions, the `xcmixin_no_hiding` check may produce false positives during compile-time validation.

This issue only affects compile-time validation and does not cause runtime errors.

| Item | Description |
|------|-------------|
| Affected compiler | MSVC |
| Affected macros | `XCMIXIN_DEF_EXTEND_BEGIN` / `XCMIXIN_IMPL_EXTEND_FOR` |
| Trigger condition | Derived class introduces base overload functions via `using Base::func` |
| Detection mechanism | `xcmixin_no_hiding` macro |
| Scope | Compile-time class validation phase only |
| Runtime behavior | Normal |

See [examples/overload-msvc-bug.cc](examples/overload-msvc-bug.cc) for details.

## License

MIT License
