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
    using methodFunc = std::function<Result(std::vector<Result>, CTContext&)>;
    virtual ~CTObject() = default;  // Virtual destructor for proper cleanup
    
    // Register and retrieve callable methods
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
    virtual std::string to_string() const = 0;

protected:
    // Map to register methods for the object
    std::unordered_map<std::string, methodFunc> methods;
};




class CTNone {};
// Value type definition
using Value = std::variant<uint64_t, double, std::string, CTFunction*, Decl*, CTNone, CTObject*, CTObject::methodFunc, void*>;

// Result type declaration
class Result {
public:
    Result();
    explicit Result(Value v);
    explicit Result(const std::string& s);
    explicit Result(const char* error_msg);

    bool is_success() const;
    std::string to_string() const;

    template <typename T>
    T* get();
private:
    bool success;
    Value value;
};

// CTArray - Derived from CTObject, representing an array-like structure
class CTArray : public CTObject {
public: 
    CTArray() {        
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
            return data[*index];
        }
        return Result("Out of range");
    }

    Result m_size(std::vector<Result> args, CTContext& context){
        return Result(data.size());
    }
    std::string to_string() const override {
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
    void    add_variable(const std::string& name, const Result& value);
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
