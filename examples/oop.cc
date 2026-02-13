#include <iostream>
#include <string>

#include "xcmixin/xcmixin.hpp"

XCMIXIN_METHOD_DEF_BEGIN(dosomethings1_method)
void dosomethings1() { std::cout << "dosomethings1" << std::endl; }
XCMIXIN_METHOD_DEF_END()
XCMIXIN_METHOD_DEF_BEGIN(name_method)
std::string name() { return "Unknown"; }
XCMIXIN_METHOD_DEF_END()

XCMIXIN_METHOD_DEF_BEGIN(new_name_method)
std::string name() { return "NewName"; }
XCMIXIN_METHOD_DEF_END()

XCMIXIN_METHOD_DEF_BEGIN(print_method)
void print() {
    xcmixin_require_method(name_method);
    std::cout << "Hello, " << xcmixin_self.name() << std::endl;
}
XCMIXIN_METHOD_DEF_END()
template <typename Base>
class MyClass;

using recorder = xcmixin::method_recorder<dosomethings1_method, print_method,
                                          name_method, new_name_method>;
template <typename Base>
class MyClass
    : public xcmixin::impl_methods_recorders<MyClass<Base>, recorder> {
    static_assert(
        xcmixin::impl_methods_recorders<MyClass<Base>, recorder>::valid_class(),
        "MyClass must be valid");
};

XCMIXIN_IMPL_METHOD_BEGIN(name_method, typename B)
XCMIXIN_IMPL_METHOD_FOR(MyClass<B>)
std::string name() { return "MyClass"; }
std::string name(int) { return "MyClass"; }
std::string name(int) const { return "MyClass"; }
XCMIXIN_METHOD_REQUIRES(
    xcmixin_no_shadow(name, std::string());
    xcmixin_no_shadow(name, std::string (MyClass<B>::*)(int));
    xcmixin_no_shadow(name, std::string (MyClass<B>::*)(int) const);

)
XCMIXIN_IMPL_METHOD_END()

int main() {
    MyClass<void> my_class;

    my_class.dosomethings1();
    my_class.name();
    my_class.print();
    return 0;
}