#include <iostream>
#include <string>

#include "xcmixin/xcmixin.hpp"
XCMIXIN_PRE_DECL(print_method)
XCMIXIN_PRE_DECL(name_method)
XCMIXIN_PRE_DECL(new_name_method)
XCMIXIN_METHOD_REQUIRE(print_method, xcmixin_no_hiding(print);
                       xcmixin_require_method(name_method););
XCMIXIN_METHOD_REQUIRE(new_name_method,
                       xcmixin_no_hiding(name, std::string(void)););
XCMIXIN_METHOD_REQUIRE(name_method,
                       xcmixin_no_hiding(name, std::string(void)););

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
    std::cout << xcmixin_self.name() << std::endl;
    std::cout << xcmixin_self.name(11) << std::endl;
    std::cout << xcmixin_const_self.name(11) << std::endl;
}
XCMIXIN_METHOD_DEF_END()
class MyClass;

XCMIXIN_IMPL_METHOD_BEGIN(name_method)
XCMIXIN_IMPL_METHOD_FOR(MyClass)
std::string name() { return "MyClass"; }
std::string name(int i) { return "MyClass " + std::to_string(i); }
std::string name(int i) const { return "const MyClass " + std::to_string(i); }
XCMIXIN_IMPL_METHOD_END()

XCMIXIN_IMPL_METHOD_BEGIN(new_name_method)
XCMIXIN_IMPL_METHOD_EXTEND_FOR(name_method, MyClass)
using base::name;
std::string name(int i) const {
    return "const new MyClass " + std::to_string(i);
}
XCMIXIN_IMPL_METHOD_END()

// XCMIXIN_IMPL_METHOD_BEGIN(new_name_method)
// XCMIXIN_IMPL_METHOD_FOR(MyClass)
// std::string name(int i) const {
//     return "const new MyClass " + std::to_string(i);
// }
// XCMIXIN_IMPL_METHOD_END()

using recorder = xcmixin::method_recorder<print_method, new_name_method,
                                          dosomethings1_method>;

class MyClass : public xcmixin::impl_methods_recorders<MyClass, recorder> {
    xcmixin_init_class;
};

template <xcmixin::Impl<print_method, name_method> T>
void print(T& p) {
    p.print();
}

int main() {
    MyClass my_class;

    print(my_class);

    return 0;
}