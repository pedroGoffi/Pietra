#ifndef PREPROCESSRESULT_CPP
#define PREPROCESSRESULT_CPP

#include "../include/preprocessresult.hpp"
#include <sstream> // For std::ostringstream

// Result class implementation
Result::Result() : success(true), value("") {}

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
                return v > 0? std::to_string(v): "0";
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << v;
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::string>) {
                return v;

            } else if constexpr (std::is_same_v<T, CTFunction*>) {
                return "CTFunction";

            } else if constexpr (std::is_same_v<T, CTNone>){
                return "None";

            } else if constexpr (std::is_same_v<T, CTObject*>){
                CTObject* object = dynamic_cast<CTObject*>(v);
                if(object){
                    return object->to_string();
                }
                return "object";

            } else if constexpr (std::is_same_v<T, CTObject::methodFunc>) {
                return "method";
            } else if constexpr (std::is_same_v<T, Decl*>) {
                Decl* decl = dynamic_cast<Decl*>(v);
                if(decl) {
                    return std::string(decl->name);
                }
                return "decl";
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

Result CTFunction::call(std::vector<Result> args, CTContext& context) const {            
    return func(args, context);
}

// CTContext class implementation
// Example for adding a variable to the current scope (either local or global)
void CTContext::add_variable(const std::string& name, Result value) {
    if(CTObject** obj_ptr = value.get<CTObject*>()){
        CTObject* obj = *obj_ptr;
        if(!obj->get_field("__name__")){
            obj->add_field("__name__", new Result(Value(obj->__name__)));
        }                
    }
    // If there are no active local scopes, add to the global variables map
    if (local_scope_stack.empty()) {    
        this->global_variables[name] = value;
    } else {
        // Otherwise, add to the current local scope
        this->variables[name] = value;
    }
}

// Method to push a new local scope
void CTContext::push_local_scope() {
    // Push the current local variables onto the stack
    local_scope_stack.push(variables);
    
    // Clear the current local variables map for the new scope
    variables.clear();
}

// Method to pop the current local scope
void CTContext::pop_local_scope() {
    // If there are any local scopes to pop
    if (!local_scope_stack.empty()) {
        // Restore the previous local variables from the stack
        variables = local_scope_stack.top();
        local_scope_stack.pop();
    } else {
        printf("Error: No local scope to pop!");
        exit(1);
    }
}

void CTContext::print_all(){
    printf("TABLE:\n");
    for (const auto& pair : this->variables) {
        const std::string& name = pair.first;
        const Result& value = pair.second;
        printf("Variable name: %s, Value: %s\n", name.c_str(), value.to_string().c_str());
    }
    printf("------------\n");
}

// Example for retrieving a variable
Result CTContext::get_variable(const std::string& name) const {
    // Check the current local scope first
    if (variables.find(name) != variables.end()) {
        return variables.at(name);
    }
    
    // If not found, check the global scope
    if (global_variables.find(name) != global_variables.end()) {
        return global_variables.at(name);
    }
    
    return Result("Error: Variable '" + name + "' not found");
}

Result* CTContext::get_variable_ptr(const std::string& name) {
    // Check the current local scope first
    if (variables.find(name) != variables.end()) {
        return &variables.at(name);
    }
    
    // If not found, check the global scope
    if (global_variables.find(name) != global_variables.end()) {
        return &global_variables.at(name);
    }
    
    return nullptr;
}
#endif /*PREPROCESSRESULT_CPP*/