# xcmixin
现代C++静态混入解决方方案

静态混入是指在编译时将多个成员函数混入到一个类中，而无需修改类的定义与实现，

**xcmixin**为现代C++提供了一种**安全**，**灵活**，**零开销**的静态混入解决方案。

## 安全
编译期验证实现/混入有效性
```cpp
class MyClass;
XCMIXIN_IMPL_AVAILABLE(MyClass); // 所有注入行为需要在类定义之前完成
// implentation
// MyClass defined
```

保证最终派生类本混入类包含依赖的全部其他混入。
```cpp
XCMIXIN_REQUIRE(print_mixin,
                       xcmixin_require_mixin(name_mixin););
```

编译期锲约，确保混入的成员函数不会隐藏基类的成员函数。或被子类覆盖，提供完善的重载选项，提供简化C++20 concept使用体验。
```cpp
XCMIXIN_REQUIRE(print_mixin,
                       xcmixin_require_method(print,void); // 确保print() [const/volatile/const volatile]  成员在派生类中存在
                       );

XCMIXIN_REQUIRE(name_mixin, xcmixin_no_hiding(name, long,int); // 确保name(int,long) [const/volatile/const volatile]  成员存在且不被隐藏
                       xcmixin_no_hiding(name, int, const_);); // 确保name(int) const成员存在且不被隐藏

```

## 灵活
提供混入方法集的通用实现。
```cpp
XCMIXIN_DEF_BEGIN(new_name_method)
std::string name() { return "NewName"; }
std::string name() const { return "NewName"; }
XCMIXIN_DEF_END()
```

在任意混入类中使用`xcmixin_self`/`xcmixin_const_self`访问全部最终混入方法。
```cpp
XCMIXIN_DEF_BEGIN(print_mixin)
void print() {
    std::cout << xcmixin_self.name() << std::endl;
    std::cout << xcmixin_self.name(11) << std::endl;
    std::cout << xcmixin_const_self.name(11) << std::endl;
}
XCMIXIN_DEF_END()
```

为特定派生类提供自定义实现。
```cpp
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyClass)
std::string name() { return "MyClass"; }
std::string name(int i) { return "MyClass " + std::to_string(i); }
std::string name(int i) const { return "const MyClass " + std::to_string(i); }
std::string name(long i) const { return "const MyClass " + std::to_string(i); }
XCMIXIN_IMPL_END()
```
基于已有混入扩展新的混入方法。
```cpp
XCMIXIN_DEF_BEGIN(name_mixin)
std::string name() { return "Unknown"; }
XCMIXIN_DEF_END()
XCMIXIN_DEF_EXTEND_BEGIN(new_name_method, name_mixin)
using base::name; // 提供base别名指代直接基类
std::string name() { return "NewName"; }
XCMIXIN_DEF_END()
```
基于已有混入为特定混入类扩展新的混入方法。
```cpp
XCMIXIN_IMPL_BEGIN(new_name_method)
XCMIXIN_IMPL_EXTEND_FOR(name_mixin, MyClass)
using base::name;
std::string name() { return "NewName"; }
XCMIXIN_IMPL_END()
```
灵活组合方法集，满足不同场景需求。
```cpp
using recorder = xcmixin::method_recorder<print_mixin, new_name_method,
                                          dosomethings1_method>;
```
基于方法记录器的混入
```cpp
class MyClass : public xcmixin::impl_recorder<MyClass, recorder> {
    xcmixin_init_class;
};
```
像调用普通成员函数一样调用混入方法。
```cpp
MyClass obj;
obj.print();
obj.name();
obj.dosomethings1();
```
替代基类引用的向上转型调用混入方法，提供类似多态的调用体验。并灵活组合不同的接口。
```cpp
template <xcmixin::Impl<print_mixin, name_mixin> T>
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

## 零开销
- 全部验证在编译期间完成，无需运行时开销。
- 生成单继承链，无多继承开销，无虚函数表开销。
- 充分利用EBO优化，注入无数据成员类时总是生成标准布局结构类。

# 应用场景
- 高性能数学计算库，利用混入提供通用数学函数，同时为特定计算场景提供定制实现。
- 所有可以编译期决定成员行为的场景，如策略模式、状态模式等。
- 部分替代OOP最佳实践，使用Impl<method>接受子类引用

# 兼容性
- clang/clang-cl 完整支持
- gcc 完整支持
- msvc 部分支持

# 已知问题
- mevc 兼容性(参见[examples\overload-msvc-bug.cc](examples\overload-msvc-bug.cc))

|||
|--|--|
|   编译器    |   MSVC                                          |
|   宏        |   `XCMIXIN_DEF_EXTEND_BEGIN`/`XCMIXIN_IMPL_EXTEND_FOR`/（扩展已有方法时）|
|   类结构    |   派生类通过 `using Base::func` 引入父类同名重载函数  |
|   验证逻辑  |   使用 `xcmixin_no_hiding` 宏检查该函数是否被隐藏     |
|   触发阶段  |   编译期类有效性验证阶段（无运行时影响）         |
|   现象     |    msvc重载决议失效，无法推断基类类型                    |

# 许可证
本项目基于MIT许可证开源，您可以在遵守许可证条款的前提下自由使用、修改和分发本项目的代码。