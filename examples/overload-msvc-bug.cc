/*
msvc 下的 bug
条件	具体场景
编译器	仅 MSVC（GCC/Clang 完全不受影响）
使用的宏	XCMIXIN_IMPL_METHOD_EXTEND_FOR（扩展已有方法时）
类结构	派生类通过 using Base::func 引入父类同名重载函数
验证逻辑	使用 xcmixin_no_hiding 宏检查该函数是否被隐藏
触发阶段	编译期 static_assert 阶段（无运行时影响）
*/

#include <type_traits>

struct A {
    int hello(int) { return 42; }
    int hello() { return 42; }
};

struct B : A {
    using A::hello;
};

struct C : B {};

template <typename C>
struct fnn {
    template <typename BaseClass, typename R>
    static constexpr auto of(R (BaseClass::*f)(int))
        requires(std::is_base_of_v<BaseClass, C> ||
                 std::is_same_v<BaseClass, C>)
    {
        return f;
    }
};

int main() {
    fnn<C>::of(&C::hello);  // MSVC fails, other compilers work
}