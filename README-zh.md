# xcmixin

[![GitHub License](https://img.shields.io/github/license/X-ChenD-Hai/xcmixin)](https://github.com/X-ChenD-Hai/xcmixin/blob/main/LICENSE)
[![GitHub Release](https://img.shields.io/github/v/release/X-ChenD-Hai/xcmixin)](https://github.com/X-ChenD-Hai/xcmixin/releases)
[![GitHub Tag](https://img.shields.io/github/v/tag/X-ChenD-Hai/xcmixin)](https://github.com/X-ChenD-Hai/xcmixin/tags)
![GitHub top language](https://img.shields.io/github/languages/top/X-ChenD-Hai/xcmixin?style=flat)

**xcmixin** 是现代 C++ 的静态混入（Mixin）解决方案，通过 CRTP 模式在编译期将多个方法混入类中，无需修改类的原始定义。

[README](README.md)

## 快速开始

### 构建项目

```bash
# 配置并构建
cmake -B build -G Ninja
cmake --build build

# 运行示例
build/examples/oop_example
```

### 快速示例

```cpp
#include <iostream>
#include <string>
#include "xcmixin/xcmixin.hpp"

class MyClass;
XCMIXIN_IMPL_AVAILABLE(MyClass);

// 1. 定义混入方法
XCMIXIN_DEF_BEGIN(name_method)
std::string name() { return "Unknown"; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_BEGIN(print_method)
void print() { std::cout << xcmixin_self.name() << std::endl; }
XCMIXIN_DEF_END()

// 2. 实现混入（可选：为特定类定制）
XCMIXIN_IMPL_BEGIN(name_method)
XCMIXIN_IMPL_FOR(MyClass)
std::string name() { return "MyClass"; }
XCMIXIN_IMPL_END()

// 3. 组合混入
using recorder = xcmixin::mixin_recorder<print_method, name_method>;

// 4. 应用到类
class MyClass : public xcmixin::impl_recorder<MyClass, recorder> {
    xcmixin_init_class;
};

int main() {
    MyClass obj;
    obj.print();      // 输出: MyClass
    obj.name();
    return 0;
}
```

## 安全

编译期验证确保混入的有效性与正确性：

**类前置声明**
```cpp
class MyClass;
XCMIXIN_IMPL_AVAILABLE(MyClass); // 所有注入行为须在类定义之前完成
// ...
class MyClass { /* ... */ };
```

**依赖约束**
```cpp
XCMIXIN_REQUIRE(print_method,
    xcmixin_require_mixin(name_method););
```

**方法签名验证**
```cpp
XCMIXIN_REQUIRE(print_method,
    xcmixin_require_method(print, void); // 验证 const/volatile/const volatile 重载
);
```

**隐藏检测**
```cpp
XCMIXIN_REQUIRE(name_method,
    xcmixin_no_hiding(name, long, int); // 验证 name(int, long) 重载不被隐藏
    xcmixin_no_hiding(name, int, const_););
```

## 灵活

### 通用实现

定义可复用的方法集合：

```cpp
XCMIXIN_DEF_BEGIN(new_name_method)
std::string name() { return "NewName"; }
std::string name() const { return "NewName"; }
XCMIXIN_DEF_END()
```

### 跨 Mixin 调用

使用 `xcmixin_self` / `xcmixin_const_self` 在任意混入中访问最终派生类的所有方法：

```cpp
XCMIXIN_DEF_BEGIN(print_method)
void print() {
    std::cout << xcmixin_self.name() << std::endl;
    std::cout << xcmixin_self.name(11) << std::endl;
    std::cout << xcmixin_const_self.name(11) << std::endl;
}
XCMIXIN_DEF_END()
```

### 特化实现

为特定派生类提供定制化的方法实现：

```cpp
XCMIXIN_IMPL_BEGIN(name_method)
XCMIXIN_IMPL_FOR(MyClass)
std::string name() { return "MyClass"; }
std::string name(int i) { return "MyClass " + std::to_string(i); }
std::string name(int i) const { return "const MyClass " + std::to_string(i); }
std::string name(long i) const { return "const MyClass " + std::to_string(i); }
XCMIXIN_IMPL_END()
```

### Mixin 扩展

基于已有 Mixin 派生新的 Mixin：

```cpp
XCMIXIN_DEF_BEGIN(name_method)
std::string name() { return "Unknown"; }
XCMIXIN_DEF_END()

XCMIXIN_DEF_EXTEND_BEGIN(new_name_method, name_method)
using base::name; // `base` 指向直接基类
std::string name() { return "NewName"; }
XCMIXIN_DEF_END()
```

### 特化扩展

为特定派生类扩展已实现的 Mixin：

```cpp
XCMIXIN_IMPL_BEGIN(new_name_method)
XCMIXIN_IMPL_EXTEND_FOR(name_method, MyClass)
using base::name;
std::string name() { return "NewName"; }
XCMIXIN_IMPL_END()
```

### 模板类支持

xcmixin 同时支持模板类。使用 `XCMIXIN_PRE_DECL` 前置声明 Mixin，并使用 `xcmixin_init_template` 进行验证：

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

`XCMIXIN_IMPL_BEGIN` 支持扩展参数以声明模板参数，可为不同模板特化提供定制实现：

```cpp
// 通用模板实现
XCMIXIN_IMPL_BEGIN(name_mixin, typename T)
XCMIXIN_IMPL_FOR(MyTemplate<T>)
std::string name() { return "MyTemplate default"; }
XCMIXIN_IMPL_END()

// MyTemplate<int> 特化
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<int>)
std::string name() { return "MyTemplate<int>"; }
XCMIXIN_IMPL_END()

// MyTemplate<float> 特化
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<float>)
std::string name() { return "MyTemplate<float>"; }
XCMIXIN_IMPL_END()

int main() {
    MyTemplate<double> obj1;
    obj1.say_hello();  // 输出: MyTemplate default hello
    MyTemplate<int> obj2;
    obj2.say_hello();  // 输出: MyTemplate<int> hello
    MyTemplate<float> obj3;
    obj3.say_hello();  // 输出: MyTemplate<float> hello
}
```

### 组合与注入

通过 `mixin_recorder` 灵活组合多个混入：

```cpp
using recorder = xcmixin::mixin_recorder<print_method, new_name_method,
                                          dosomethings1_method>;
```

将组合后的 Mixin 注入目标类：

```cpp
class MyClass : public xcmixin::impl_recorder<MyClass, recorder> {
    xcmixin_init_class;  // 须在类定义末尾调用，执行编译期验证
};
```

### 使用方式

与普通成员函数无异：

```cpp
MyClass obj;
obj.print();
obj.name();
obj.dosomethings1();
```

### 泛型约束

使用 `Impl` 概念约束模板类型，作为传统基类引用的替代与增强：

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

相较于传统基类引用，`Impl` 概念无需实际继承关系，仅需派生类包含指定的 Mixin 注入即可，提供了更灵活的约束方式。

## 零开销

- **编译期完成**：所有验证在编译期间完成，无运行时开销
- **单一继承链**：生成单继承结构，无多继承或虚函数表开销
- **EBO 优化**：无数据成员的 Mixin 采用空基类优化，始终保持标准布局

## 应用场景

- **高性能数学库**：为通用计算提供默认实现，为特定场景提供定制优化
- **策略模式**：编译期决定策略行为，消除运行时分派开销
- **状态模式**：编译期状态管理，无运行时状态对象开销
- **接口组合**：按需组合不同功能模块

## 兼容性

| 编译器 | 支持情况 |
|--------|----------|
| clang / clang-cl | 完整支持 |
| gcc | 完整支持 |
| MSVC | 部分支持 |

## 已知问题

### MSVC 重载决议缺陷

MSVC 编译器在处理扩展混入宏时存在重载决议缺陷。当使用 `XCMIXIN_DEF_EXTEND_BEGIN` 或 `XCMIXIN_IMPL_EXTEND_FOR` 宏，并通过 `using base::func` 语法引入基类重载函数时，`xcmixin_no_hiding` 检测会在编译期产生误判。

此问题仅影响编译期的有效性验证，不会导致运行时错误。

| 项目 | 说明 |
|------|------|
| 受影响编译器 | MSVC |
| 受影响宏 | `XCMIXIN_DEF_EXTEND_BEGIN` / `XCMIXIN_IMPL_EXTEND_FOR` |
| 触发条件 | 派生类通过 `using Base::func` 引入基类同名重载函数 |
| 检测机制 | `xcmixin_no_hiding` 宏 |
| 影响范围 | 仅编译期类有效性验证阶段 |
| 运行时行为 | 正常 |

详细说明请参考 [examples/overload-msvc-bug.cc](examples/overload-msvc-bug.cc)。

## 许可证

MIT License
