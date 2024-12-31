#ifndef PREPROCESSRESULT_CPP
#define PREPROCESSRESULT_CPP
#include "../include/preprocessresult.hpp"
#include <sstream> // For std::ostringstream

// Result class implementation
Result::Result() : success(false), value("") {}

Result::Result(Value v) : success(true), value(v) {}

Result::Result(const std::string& s) : success(true), value(s) {}

Result::Result(const char* error_msg) : success(false), value(std::string(error_msg)) {}

bool Result::is_success() const {
    return success;
}
template <typename T>
T* Result::get(){    
    if (auto ptr = std::get_if<T>(&value)) {
        return ptr;
    }
    return nullptr;  // Return nullptr if the type doesn't match
}
std::string Result::to_string() const {
    if (success) {
        return std::visit([](auto&& v) -> std::string {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, uint64_t>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << v;
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else if constexpr (std::is_same_v<T, CTFunction*>) {
                return "CTFunction";
            } else if constexpr (std::is_same_v<T, Decl*>) {
                return "Function";
            }
            assert(0);
        }, value);
    } else {
        return "Error: " + std::get<std::string>(value);
    }
}

// CTFunction class implementation
CTFunction::CTFunction(const std::string& name, FuncType func)
    : name(name), func(func) {}

const std::string& CTFunction::get_name() const {
    return name;
}

Result CTFunction::call(std::vector<Expr*> args, CTContext& context) const {    
    return func(args, context);
}

// CTContext class implementation
void CTContext::add_variable(const std::string& name, const Result& value) {
    variables[name] = value;
}

Result* CTContext::get_variable_ptr(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return &it->second;
    }
    return nullptr;

}
void CTContext::print_all(){
    for (const auto& pair : this->variables) {
        const std::string& name = pair.first;
        const Result& value = pair.second;
        printf("Variable name: %s, Value: %s\n", name.c_str(), value.to_string().c_str());
    }
}
Result CTContext::get_variable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return Result("Error: Variable not found");
}

#endif /*PREPROCESSRESULT_CPP*/