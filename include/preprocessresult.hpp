#ifndef PREPROCESS_RESULT
#define PREPROCESS_RESULT
#include <stack>
#include "smallVec.hpp"
#include "ast.hpp"
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>
#include <cstdint>
#include <functional>
#include <sstream> // For std::ostringstream
#include <fstream>   // For std::fstream
#include <sstream>   // For std::ostringstream
#include <stdexcept> // For std::runtime_error
#include <vector>    // For std::vector
#include <string>    // For std::string
#include <iostream>  // (Optional) For debugging with std::cout

using namespace Pietra;
using namespace Pietra::Ast;
// Enum to define the status of the result
enum class Status {
    Success,
    Error
};

// Forward declarations
class Result;
class CTContext;
class CTFunction;


Result evaluate_int_expr(Expr* expr);
Result evaluate_float_expr(Expr* expr);
Result evaluate_string_expr(Expr* expr);
Result evaluate_name_expr(Expr* expr, CTContext& context);
Result evaluate_reassing(Expr* expr, CTContext& context);
Result evaluate_binary_expr(Expr* expr, CTContext& context);
Result evaluate_call_expr(Expr* expr, CTContext& context);
Result evaluate_init_var_expr(Expr* expr, CTContext& context);
Result evaluate_field_expr(Expr* expr, CTContext& context);
Result evaluate_binary_mul(Result lhs, Result rhs, CTContext& context);
Result evaluate_binary_lesss(Result lhs, Result rhs, CTContext& context);
Result evaluate_binary_plus(Result lhs, Result rhs, CTContext& context);
Result evaluate_binary_minus(Result lhs, Result rhs, CTContext& context);
Result evaluate_expr(Expr* expr, CTContext& context);
  // Function to preprocess different types of statements
Result prep_stmt_expr(Stmt* stmt, CTContext& context);
Result prep_stmt_if(Stmt* stmt, CTContext& context);
Result prep_stmt_switch(Stmt* stmt, CTContext& context);
Result prep_stmt_for(Stmt* stmt, CTContext& context);
Result prep_stmt_while(Stmt* stmt, CTContext& context);
Result prep_stmt_return(Stmt* stmt, CTContext& context);
Result prep_stmt(Stmt* stmt, CTContext& context); // General handler for statement preprocessing
Result prep_block(SVec<Stmt*> block, CTContext& context);
Result evaluate_decl_call(Decl* decl, std::vector<Result> args, CTContext& context);
Result prep_decl(Decl* decl, CTContext& context);
Result handle_var_decl(Decl* decl, CTContext& context);
Result handle_proc_decl(Decl* decl, CTContext& context);
Result handle_aggregate_decl(Decl* decl, CTContext& context);
Result handle_enum_decl(Decl* decl, CTContext& context);
Result handle_type_alias_decl(Decl* decl, CTContext& context);
Result handle_use_decl(Decl* decl, CTContext& context);
Result handle_impl_decl(Decl* decl, CTContext& context);
Result prep_run_ast(std::vector<Decl*> ast);
    


// Base class for all objects

class CTObject {
public:
    const char* __name__;
    using methodFunc = std::function<Result(std::vector<Result>, CTContext&)>;

    //CTObject() : __name__("object") {}
    CTObject(const char* __name__) : __name__(__name__) {}
    virtual ~CTObject() = default;  // Virtual destructor for proper cleanup
            
    void add_method(std::string name, methodFunc func){
        assert(!this->get_method(name));
        this->unsafe_set_method(name, func);        
    }
    void unsafe_set_method(std::string name, methodFunc func){
        this->methods[name] = func;
    }

    methodFunc get_method(const std::string& method_name) {
        if (methods.find(method_name) != methods.end()) {
            return methods[method_name];
        } else {
            return nullptr;
        }
    }

    // Virtual to_string method for debugging or printing
    
    void add_field(std::string name, Result* result){
        assert(!this->get_field(name));
        this->unsafe_set_field(name, result);
    }
    void unsafe_set_field(std::string name, Result* result){
        this->fields[name] = result;
    }
    Result* get_field(std::string name){
        if(this->fields.find(name) != this->fields.end()){
            return this->fields[name];
        }
        return nullptr;
    }    

    std::string to_string() const { return "object"; }
protected:
    // Map to register methods for the object
    std::unordered_map<std::string, methodFunc> methods;
    std::unordered_map<std::string, Result*>     fields;        
};

class CTNone {};
class CTStream;
// Value type definition
using RTValue = std::variant<uint64_t, double, std::string, CTFunction*, Decl*, CTNone, CTObject*, CTObject::methodFunc, void*>;

// Result type declaration
class Result {
public:
    Result();
    explicit Result(RTValue v);
    explicit Result(const std::string& s);
    explicit Result(const char* error_msg);

    bool is_returning = false;
    bool is_success() const;
    std::string to_string() const;

    template <typename T>
    T* get();

    bool is_ctnone() const {
        return std::holds_alternative<CTNone>(value);
    }

    bool is_true_if_cond(){
        if (std::holds_alternative<uint64_t>(value)) {
            return std::get<uint64_t>(value) != 0;
        }
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) != 0.0;
        }
        if (std::holds_alternative<std::string>(value)) {
            return !std::get<std::string>(value).empty();
        }
        if (std::holds_alternative<CTObject*>(value)) {
            return std::get<CTObject*>(value) != nullptr;
        }
        if (std::holds_alternative<CTNone>(value)) {
            return false; // CTNone is always false
        }
        if (std::holds_alternative<void*>(value)) {
            return std::get<void*>(value) != nullptr;
        }

        // Handle other types if needed
        printf("Unknown type in if condition\n");
        return false;
    }
private:
    bool success;
    RTValue value;
};

// CTArray - Derived from CTObject, representing an array-like structure
class CTArray : public CTObject {
public: 
    CTArray() : CTObject("builtin_array")
    {
        this->add_method("push", [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_push(args, context);});
        this->add_method("repr", [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_repr(args, context);});
        this->add_method("size", [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_size(args, context);});
        this->add_method("at",  [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_at(args, context);});
        this->add_method("begin",  [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_begin(args, context);});
        this->add_method("end",  [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_end(args, context);});
        
    }

    Result m_end(std::vector<Result> args, CTContext& context) {
        return data.begin() != data.end()? *(data.end() - 1): this->m_begin(args, context);
        
    }

    Result m_begin(std::vector<Result> args, CTContext& context) {
        return *this->data.begin();
    }

    Result m_repr(std::vector<Result> args, CTContext& context) {
        return Result(this->to_string());
    }
    
    Result m_push(std::vector<Result> args, CTContext& context) {
        if (args.size() == 0) {
            return Result("vector push expected at least one argument");
        }

        for(Result arg: args){
            data.push_back(arg);
        }
        
        return Result((uint64_t) data.size());
    }
    // Get the element by index
    Result m_at(std::vector<Result> args, CTContext& context){
        if(args.size() != 1){
            return Result("Expected 1 argument as index");
        }
        uint64_t* index = args.at(0).get<uint64_t>();
        if(!index){
            return Result("bad argument");
        }
        if (*index < data.size()) {
            return this->data[*index];
        }
        return Result("Out of range");
    }

    Result m_size(std::vector<Result> args, CTContext& context){
        return Result((uint64_t) data.size());
    }
    std::string to_string() const {
        std::ostringstream oss;
        oss << "[";

        for (size_t i = 0; i < data.size(); ++i) {
            // Convert each element to string and append
            oss << data[i].to_string();
            if (i < data.size() - 1) {
                oss << ", ";  // Add a separator between elements
            }
        }

        oss << "]";
        return oss.str();
    }


private:
    std::vector<Result> data;  // The actual array data
};

class CTStream : public CTObject {
public:
    CTStream(const std::string& filename, const std::string& mode) : CTObject("builtin_stream"), file(filename, get_mode(mode)) {
        if (!file.is_open()) {
            return;
        }

        printf("Iniciando CTSTream\n")    ;
        this->add_method("write",   [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_write(args, context); });
        this->add_method("read",    [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_read(args, context); });
        this->add_method("close",   [&](std::vector<Result> args, CTContext& context) -> Result { return this->m_close(args, context); });
    }

    

    Result m_write(std::vector<Result> args, CTContext& context) {
        if (args.empty()) {
            return Result("Expected at least one argument to write");
        }
        for (const auto& arg : args) {
            file << arg.to_string();
        }
        file.flush();
        return Result((uint64_t)1);
    }

    Result m_read(std::vector<Result> args, CTContext& context) {
        if (!file.is_open()) {
            return Result("File is not open");
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        return Result(ss.str());
    }

    Result m_close(std::vector<Result> args, CTContext& context) {
        if (file.is_open()) {
            file.close();
        }
        return Result((uint64_t)1);
    }

private:
    std::fstream file;
    std::ios_base::openmode get_mode(const std::string& mode) {
        if (mode == "r") return std::ios::in;
        if (mode == "w") return std::ios::out | std::ios::trunc;
        if (mode == "a") return std::ios::app;
        if (mode == "rw") return std::ios::in | std::ios::out;
        
        std::cerr << "Invalid mode: " << mode << std::endl; // Debugging
        return std::ios::in; // Default
    }
};

// CTFunction class declaration
class CTFunction {
public:
    using FuncType = std::function<Result(std::vector<Result>, CTContext&)>;

    CTFunction(const std::string& name, FuncType func);

    const std::string& get_name() const;
    Result call(std::vector<Result> args, CTContext& context) const;

private:
    std::string name;
    FuncType func;
};

// CTContext class declaration
class CTContext {
public:
    Result* get_variable_ptr(const std::string& name);
    void    add_variable(const std::string& name, Result value);
    Result  get_variable(const std::string& name) const;
    void    print_all();

    // Manage scope for function calls
    void    push_local_scope();  // Push a new local scope
    void    pop_local_scope();   // Pop the current local scope

private:
    // Local variable storage for the current scope
    std::unordered_map<std::string, Result> variables;
    
    // Stack to manage different local scopes
    std::stack<std::unordered_map<std::string, Result>> local_scope_stack;

    // Global variables (persistent across all scopes)
    std::unordered_map<std::string, Result> global_variables;
};


#endif // PREPROCESS_RESULT
