#include <iostream>
#include <string>
#include <xcmixin/xcmixin.hpp>

XCMIXIN_DEF_BEGIN(logable)
// logable(std::string name_scope) : name_scope_(std::move(name_scope)) {}
void log(std::string_view msg) const {
    std::cout << "log from " << name_scope_ << " msg: " << msg << std::endl;
}

private:
std::string name_scope_{};
XCMIXIN_DEF_END()

class MyClass : public xcmixin::impl_mixin<MyClass, logable> {
   public:
    MyClass(std::string name_scope) : xcmixin_self_class(
        

    ) {}
    xcmixin_init_class;
};

int main() {
    MyClass my_class{"my_class"};
    my_class.log("hello world");
    return 0;
}