#include <iostream>

#include "xcmixin/xcmixin.hpp"

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
// Unlike regular classes, mixin implementations can be injected into template
// classes at any point before template instantiation
XCMIXIN_IMPL_BEGIN(name_mixin,
                   typename T)  // Extended args declare template parameters
XCMIXIN_IMPL_FOR(
    MyTemplate<T>)  // Use pre-declared template args to specify class
std::string name() { return "MyTemplate default"; }
XCMIXIN_IMPL_END()

// Provide specialized implementation for MyTemplate<int>
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<int>)
std::string name() { return "MyTemplate<int>"; }
XCMIXIN_IMPL_END()

// Provide specialized implementation for MyTemplate<float>
XCMIXIN_IMPL_BEGIN(name_mixin)
XCMIXIN_IMPL_FOR(MyTemplate<float>)
std::string name() { return "MyTemplate<float>"; }
XCMIXIN_IMPL_END()

int main() {
    MyTemplate<double> my_template_d;
    my_template_d.say_hello();
    MyTemplate<int> my_template_i;
    my_template_i.say_hello();
    MyTemplate<float> my_template_f;
    my_template_f.say_hello();

    return 0;
}