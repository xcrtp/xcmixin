#include <iostream>
#include <string>
#include <xcmixin/xcmixin.hpp>

XCMIXIN_DEF_BEGIN(logable2)
XCMIXIN_CONSTRUCTOR(logable2, std::string name_scope1, std::string name_scope2)
, name_scope1_(std::move(name_scope1)), name_scope2_(std::move(name_scope2)) {}
void log2(std::string_view msg) const {
    std::cout << "log2 from " << name_scope1_ << " msg: " << msg << std::endl;
    std::cout << "log2 from " << name_scope2_ << " msg: " << msg << std::endl;
}

private:
std::string name_scope1_{};
std::string name_scope2_{};
XCMIXIN_DEF_END()
XCMIXIN_DEF_BEGIN(logable)
XCMIXIN_CONSTRUCTOR(logable, std::string name_scope)
, name_scope_(std::move(name_scope)) {}
template <typename = void>
void log(std::string_view msg) const {
    std::cout << "log from " << name_scope_ << " msg: " << msg << std::endl;
}
private:
std::string name_scope_{};
XCMIXIN_DEF_END()

class MyClass : public xcmixin::impl_mixin<MyClass, logable, logable2> {
   public:
    template <typename... Args>
    MyClass(Args&&... args) : xcmixin_self_class(std::forward<Args>(args)...) {}
    xcmixin_init_class;
};

int main() {
    MyClass c("my_class", "logable2", "logable");

    c.log("hello world");
    c.log2("hello world");

    return 0;
}