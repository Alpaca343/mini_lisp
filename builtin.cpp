#include "./builtin.h"

const std::unordered_map<std::string, BuiltinFuncType*>& getBuiltins() {
    static std::unordered_map<std::string, BuiltinFuncType*> m =
    {  // 用static避免反复创建新的临时变量，同时避免悬垂引用
        {"+", add}, {"print", print}, 
    };
    return m;
}

ValuePtr add(const std::vector<ValuePtr>& params) {
    auto result = 0.0;
    for (const auto& i : params) {
        if (auto num = i->asNumber()) {
            result += *num;
        } else {
            throw LispError("Cannot add a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr print(const std::vector<ValuePtr>& params) {
    for (const auto& i : params) {
        std::cout << i->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}
