#ifndef PREPROCESS_CPP
#define PREPROCESS_CPP
#include "../include/smallVec.hpp"
#include "../include/ast.hpp"
#include "../include/interns.hpp"
#include "preprocessresult.cpp"
#include <string.h>


using namespace Ast;
    

CTContext theContext;
const char* format_impl_name(const char* parent, const char* impl){
    return strf("%s_impl_%s", parent, impl);
}

    // Helper function to evaluate an integer expression
Result evaluate_int_expr(Expr* expr) {
    return Result(expr->int_lit);
}

// Helper function to evaluate a float expression
Result evaluate_float_expr(Expr* expr) {
    return Result(expr->float_lit);
}

// Helper function to evaluate a string expression
Result evaluate_string_expr(Expr* expr) {
    return Result(std::string(expr->string_lit));
}

// Helper function to evaluate a variable name expression
Result evaluate_name_expr(Expr* expr, CTContext& context) {
    return context.get_variable(std::string(expr->name));  // Retrieve variable from context
}

Result* get_field_addr(Expr* expr, CTContext& context){
    Result parent = evaluate_expr(expr->field_acc.parent, context);        
    Expr* field_expr = expr->field_acc.children;
    assert(field_expr->kind == EXPR_NAME);
    const char* field = field_expr->name;
    
        
    if(CTObject** object_ptr = parent.get<CTObject*>()){
        CTObject* object = *object_ptr;        
        if(Result* field_result = object->get_field(field)) {
            return field_result;
        }
        
        return nullptr;
    }

    
    //if(Decl** decl_ptr = parent.get<Decl*>()){
    //    Decl* decl = *decl_ptr;
    //    if(decl->kind == DECL_ENUM) return evaluate_field_decl_enum(decl, field, context);        
    //}

    printf("GOT %s\n", parent.to_string().c_str());
    assert(0 && "address not found");
}

Result evaluate_reassing(Expr* expr, CTContext& context){
    Expr* lhs   = expr->binary.left;
    Result rhs  = evaluate_expr(expr->binary.right, context);
    if(lhs->kind == EXPR_NAME) {
        context.add_variable(lhs->name, rhs);
        return rhs;
    }
    if(lhs->kind == EXPR_FIELD) {
        Result* addr = get_field_addr(expr->binary.left, context);
        if(!addr){
            printf("addr not found bruh\n");
            return Result("reassign field error");
        }

        *addr = rhs;
        return *addr;
    }

    else {
        printf("LHS = %zu\n", expr->binary.left->kind);
        return Result("Failed to reassign");
    }
}
bool is_cmp_tokenkind(TokenKind kind){
    return kind == TK_eqeq || kind == TK_noteq || kind == TK_less || kind == TK_lesseq || kind == TK_greater || kind == TK_greatereq;
}
Result evaluate_cmp_eq(Result lhs, Result rhs, CTContext& context) {
    if (uint64_t* lhs_int = lhs.get<uint64_t>()) {
        if (uint64_t* rhs_int = rhs.get<uint64_t>()) {
            return Result((uint64_t)(*lhs_int == *rhs_int));  // Both are integers, perform equality comparison
        } else if (double* rhs_double = rhs.get<double>()) {
            return Result((uint64_t)(static_cast<double>(*lhs_int) == *rhs_double));  // lhs is int, rhs is double
        }
    } else if (double* lhs_double = lhs.get<double>()) {
        if (double* rhs_double = rhs.get<double>()) {
            return Result((uint64_t)(*lhs_double == *rhs_double));  // Both are doubles, perform equality comparison
        } else if (uint64_t* rhs_int = rhs.get<uint64_t>()) {
            return Result((uint64_t)(*lhs_double == static_cast<double>(*rhs_int)));  // lhs is double, rhs is int
        }
    } else if (std::string* lhs_str = lhs.get<std::string>()) {
        if (std::string* rhs_str = rhs.get<std::string>()) {
            return Result((uint64_t)(*lhs_str == *rhs_str));  // Both are strings, perform equality comparison
        }
    }
    return Result("Error: Invalid types for equality comparison");
}

Result evaluate_and_expr(Result lhs, Result rhs, CTContext& context) {
    if (uint64_t* lhs_int = lhs.get<uint64_t>()) {
        if (uint64_t* rhs_int = rhs.get<uint64_t>()) {
            return Result((uint64_t)(*lhs_int && *rhs_int));  // Both are integers, perform AND operation
        }
    }
    return Result("Error: Invalid types for AND operation");
}
Result evaluate_cmp(TokenKind op, Result lhs, Result rhs, CTContext& context) {
    if(op == TK_eqeq) return evaluate_cmp_eq(lhs, rhs, context);
    return Result("Error: Unsupported operator for comparison: " + std::string(tokenKindRepr(op)));
}
Result evaluate_binary_mul(Result lhs, Result rhs, CTContext& context) {
    // Try to get the values of lhs and rhs as specific types
    if (uint64_t* lhs_int = lhs.get<uint64_t>()) {
        if (uint64_t* rhs_int = rhs.get<uint64_t>()) {
            // Both are integers, perform multiplication
            return Result((*lhs_int) * (*rhs_int));
        }

        if (double* rhs_double = rhs.get<double>()) {
            // lhs is int, rhs is double
            return Result(static_cast<double>(*lhs_int) * (*rhs_double));
        }
    }

    if (double* lhs_double = lhs.get<double>()) {
        if (double* rhs_double = rhs.get<double>()) {
            // Both are doubles, perform multiplication
            return Result((*lhs_double) * (*rhs_double));
        }

        if (uint64_t* rhs_int = rhs.get<uint64_t>()) {
            // lhs is double, rhs is int
            return Result((*lhs_double) * static_cast<double>(*rhs_int));
        }
    }

    // Unsupported operand types, return an error
    return Result("Error: Unsupported operand types for binary multiplication");
}

Result evaluate_binary_lesss(Result lhs, Result rhs, CTContext& context) {
    // Try to get values of lhs and rhs as specific types
    if (auto lhs_int = lhs.get<uint64_t>()) {
        if (auto rhs_int = rhs.get<uint64_t>()) {
            return Result((uint64_t)(*lhs_int < *rhs_int));  // Both are integers, perform less-than comparison
        } else if (auto rhs_double = rhs.get<double>()) {
            return Result((uint64_t)(static_cast<double>(*lhs_int) < *rhs_double));  // lhs is int, rhs is double
        }
    } else if (auto lhs_double = lhs.get<double>()) {
        if (auto rhs_double = rhs.get<double>()) {
            return Result((uint64_t)(*lhs_double < *rhs_double));  // Both are doubles, perform less-than comparison
        } else if (auto rhs_int = rhs.get<uint64_t>()) {
            return Result((uint64_t)(*lhs_double < static_cast<double>(*rhs_int)));  // lhs is double, rhs is int
        }
    } else if (auto lhs_str = lhs.get<std::string>()) {
        if (auto rhs_str = rhs.get<std::string>()) {
            return Result((uint64_t)(*lhs_str < *rhs_str));  // Both are strings, perform lexicographical less-than comparison
        }
    }

    
    return Result("Error: Invalid types for binary < operation");
}
Decl* object_has_constructor(CTObject* object, CTContext& context){
    if(Result* constructor_ptr = context.get_variable_ptr(format_impl_name(object->__name__, object->__name__))){
        if(Decl** constructor_decl = constructor_ptr->get<Decl*>()){
            return *constructor_decl;
        }
    }
    
    return nullptr;
}
Result evaluate_binary_minus(Result lhs, Result rhs, CTContext& context) {
    // Check if both results are of the same type (either integer or double)
    if (lhs.is_success() && rhs.is_success()) {
        // Check if both are integers
        if (auto lhs_int = lhs.get<uint64_t>()) {
            if (auto rhs_int = rhs.get<uint64_t>()) {
                return Result(*lhs_int - *rhs_int);
            }
        }
        // Check if both are doubles
        else if (auto lhs_double = lhs.get<double>()) {
            if (auto rhs_double = rhs.get<double>()) {
                return Result(*lhs_double - *rhs_double);
            }
        }
    }

    // If types do not match or the operation failed, return an error
    return Result("Error: Invalid types for subtraction.");
}

Result evaluate_binary_plus(Result lhs, Result rhs, CTContext& context) {
    // Try to get values of lhs and rhs as specific types
    if (auto lhs_int = lhs.get<uint64_t>()) {
        if (auto rhs_int = rhs.get<uint64_t>()) {
            return Result(*lhs_int + *rhs_int);  // Both are integers, perform addition
        } else if (auto rhs_double = rhs.get<double>()) {
            return Result(static_cast<double>(*lhs_int) + *rhs_double);  // lhs is int, rhs is double
        }
    } else if (auto lhs_double = lhs.get<double>()) {
        if (auto rhs_double = rhs.get<double>()) {
            return Result(*lhs_double + *rhs_double);  // Both are doubles, perform addition
        } else if (auto rhs_int = rhs.get<uint64_t>()) {
            return Result(*lhs_double + static_cast<double>(*rhs_int));  // lhs is double, rhs is int
        }
    } else if (auto lhs_str = lhs.get<std::string>()) {
        if (auto rhs_str = rhs.get<std::string>()) {
            return Result(*lhs_str + *rhs_str);  // Both are strings, perform concatenation
        }
    }

    // If types do not match or cannot perform the operation, return an error
    return Result("Error: Invalid types for binary + operation");
}

// Helper function to evaluate an initialization expression
Result evaluate_binary_expr(Expr* expr, CTContext& context) {
    if(expr->binary.binary_kind == TK_eq) return evaluate_reassing(expr, context);
    // Evaluate left operand
    Result lhs = evaluate_expr(expr->binary.left, context);
    
    if (!lhs.is_success()) {
        printf("Error: failed to evaluate left operand");
        exit(1);
        return Result("Error: failed to evaluate left operand");
    }
    // Evaluate right operand
    Result rhs = evaluate_expr(expr->binary.right, context);
    
    if (!rhs.is_success()) {
        return Result("Error: failed to evaluate right operand");
    }
    TokenKind op = expr->binary.binary_kind;    
    if(op == TK_plus)   return evaluate_binary_plus(lhs, rhs, context);
    if(op == TK_minus)  return evaluate_binary_minus(lhs, rhs, context);
    if(op == TK_less)   return evaluate_binary_lesss(lhs, rhs, context);
    if(op == TK_mul)    return evaluate_binary_mul(lhs, rhs, context);
    if(is_cmp_tokenkind(op)) 
        return evaluate_cmp(op, lhs, rhs, context);
    
    if(op == TK_andand)    return evaluate_and_expr(lhs, rhs, context);

    
    
    printf("OP  = %s\n", tokenKindRepr(op));
    return Result("Error: unknown operator");
    
}
void setup_aggregate_object(CTObject* object, Decl* decl){
    for(AggregateItem* item: decl->aggregate.items){
        for(const char* field: item->field.names){            
            Result* field_result = nullptr;

            if(item->field.init){
                assert(0 && "TODO");
            }
            else {
                field_result = new Result(Value("None"));
            }            
            object->add_field(std::string(field), field_result);
        }
        
    }
}
Result evaluate_call_aggr(Decl* decl, std::vector<Result> args, CTContext& context) {
    assert(decl->kind == DECL_AGGREGATE);
    Result agg = context.get_variable(decl->name);
    CTObject* object = new CTObject(decl->name);
    setup_aggregate_object(object, decl);    
    return Result(object);
}
Result evaluate_decl_call(Decl* decl, std::vector<Result> args, CTContext& context) {    
    assert(decl->kind == DECL_PROC);

    // Push local scope for the function call
    
    context.push_local_scope();
    
    // Setup the parameters
    size_t params_count = decl->proc.params.len();
    size_t args_count = args.size();
    
    // Check if the arguments count matches the parameters count
    if (params_count != args_count) {
        return Result("Arguments count and parameters count don't match in function: " + std::string(decl->name));
    }

    // Add parameters to the local scope
    for (size_t i = 0; i < args_count; i++) {        
        ProcParam* param = decl->proc.params.at(i);    
        Result arg_result = args.at(i);
        context.add_variable(std::string(param->name), arg_result); // Add to current scope
    }
    
    // Now that parameters are added, execute the function body (block)
    
    Result got_result  = prep_block(decl->proc.block, context); // Execute function body
    
    // Pop the local scope after the function execution
    context.pop_local_scope();

    
    return got_result;
}

Result evaluate_call_expr(Expr* expr, CTContext& context) {    
    Result base = evaluate_expr(expr->call.base, context);
    if(!base.is_success()) return Result("Error: failed to call function");
    
    std::vector<Result> args;
    for(Expr* e_arg: expr->call.args){
        Result arg = evaluate_expr(e_arg, context);
        if(!arg.is_success()) {
            printf("GOT: %s\n", arg.to_string().c_str());
            return Result("ERROR: argument call");
        }

        args.push_back(arg);
    }

    if(CTFunction** func_ptr = base.get<CTFunction*>()){
        CTFunction* func = *func_ptr;
        assert(func);        
        return func->call(args, context);
    } 
    else if(Decl** decl_ptr = base.get<Decl*>()){
        Decl* decl = *decl_ptr;
        if (strstr(decl->name, "_impl_")) {            
            assert(expr->call.base->kind == EXPR_FIELD);
            // Damn thats a way to deal with memory 
            Result self = evaluate_expr(expr->call.base->field_acc.parent, context);
            args.insert(args.begin(), self);
        }

        if(decl->kind == DECL_PROC)         return evaluate_decl_call(decl, args, context);        
        if(decl->kind == DECL_AGGREGATE)    return evaluate_call_aggr(decl, args, context);
    
        
        

        return Result("cant handle this declaration in a call context: " + std::string(decl->name));
        
    }
    else if(CTObject::methodFunc* method = base.get<CTObject::methodFunc>()){
        return (*method)(args, context);
    }
    
    else {
        printf("ERR: %s\n", base.to_string().c_str());
        assert(0 && "bad func");
    }
    return Result();
}

Result evaluate_field_decl_enum(Decl* decl, const char* field, CTContext& context){
    assert(decl->kind == DECL_ENUM);
    uint64_t    iota = 0;
    bool        found = false;
    for(EnumItem* item: decl->enumeration.items){        
        if(item->name == field){                        
            found = true;
            break;
        }
        iota++;
    } 

    if(!found) return Result("enum item not found");

    return Result(iota);
}
Result evaluate_unary_expr(Expr* expr, CTContext& context){
    Result operand = evaluate_expr(expr->unary.expr, context);
    if(expr->unary.unary_kind == TK_minus){
        if(uint64_t* int_ptr = operand.get<uint64_t>()){
            return Result(-(*int_ptr));
        }
        if(double* double_ptr = operand.get<double>()){
            return Result(-(*double_ptr));
        }
    }
    return Result("unary error");
}
Result evaluate_lambda_expr(Expr* expr, CTContext& context){
    return Result(new CTFunction("lambda", [=](std::vector<Result> args, CTContext& context){
        context.push_local_scope();
        for(size_t i = 0; i < args.size(); i++){
            context.add_variable(expr->lambda.params.at(i)->name, args.at(i));
        }
        Result got_result = prep_block(expr->lambda.block, context);
        context.pop_local_scope();
        return got_result;
    }));
}
Result evaluate_field_expr(Expr* expr, CTContext& context){
    Result parent = evaluate_expr(expr->field_acc.parent, context);        
    Expr* field_expr = expr->field_acc.children;
    assert(field_expr->kind == EXPR_NAME);
    const char* field = field_expr->name;
    if(field == cstr("is_error")){
        return Result((uint64_t)(!parent.is_success()));
    } else if(field == cstr("is_none")){        
        return Result((uint64_t)(parent.get<CTNone>() != nullptr));
    } else if(field == cstr("error")) {
        return Result(parent.to_string());
    }
        
    if(CTObject** object_ptr = parent.get<CTObject*>()){
        CTObject* object = *object_ptr;
        if(CTObject::methodFunc func = object->get_method(field))  return Result(func);
        if(Result* field_result = object->get_field(field))               return *field_result;                
        if(Result* impl_method = context.get_variable_ptr(format_impl_name(object->__name__, field))){
            return *impl_method;
        }
        

        printf("IDK BRUH: %s\n", object->to_string().c_str());
        assert(0);
    }
    if(Decl** decl_ptr = parent.get<Decl*>()){
        Decl* decl = *decl_ptr;
        if(decl->kind == DECL_ENUM) return evaluate_field_decl_enum(decl, field, context);        
    }

    printf("GOT %s\n", parent.to_string().c_str());
    assert(0 && "field not found");
}
Result evaluate_init_var_expr(Expr* expr, CTContext& context) {
    Result rhs_value = evaluate_expr(expr->init_var.rhs, context);
    context.add_variable(std::string(expr->init_var.name), rhs_value);  // Store the initialized variable in context
    Result* val = context.get_variable_ptr(expr->init_var.name);
    assert(val);
    
    if(CTObject** obj_ptr = rhs_value.get<CTObject*>()){
        CTObject* obj = *obj_ptr;
        if(Decl* constructor = object_has_constructor(obj, context)){        
            std::vector<Result> args;
            args.push_back(rhs_value);
            
            //printf("VISITING CONSTRUCTOR OF %s\n", obj->__name__);
            evaluate_decl_call(constructor, args, context);
            
        }
        printf("Allocating a object1\n");
    }
    return rhs_value;
}

// Generic function to evaluate an expression
Result evaluate_expr(Expr* expr, CTContext& context) {
    switch (expr->kind) {
        case EXPR_INT:          return evaluate_int_expr(expr);
        case EXPR_FLOAT:        return evaluate_float_expr(expr);
        case EXPR_STRING:       return evaluate_string_expr(expr);
        case EXPR_NAME:         return evaluate_name_expr(expr, context);
        case EXPR_INIT_VAR:     return evaluate_init_var_expr(expr, context);
        case EXPR_CALL:         return evaluate_call_expr(expr, context);
        case EXPR_BINARY:       return evaluate_binary_expr(expr, context);
        case EXPR_FIELD:        return evaluate_field_expr(expr, context);
        case EXPR_LAMBDA:       return evaluate_lambda_expr(expr, context);
        case EXPR_UNARY:        return evaluate_unary_expr(expr, context);
        // Add additional cases here, using helper functions for each kind        
        default: 
            printf("EXPR kind = %zu\n", expr->kind);
            assert(0 && "unimplemented");
    }
}
    
// Preprocess expressions in a statement
Result prep_stmt_expr(Stmt* stmt, CTContext& context) {
    return evaluate_expr(stmt->expr, context);
    
    
}
// Preprocess the 'if' statement
Result prep_stmt_if(Stmt* stmt, CTContext& context) {
    // Process the 'if' condition
    Result cond_result = evaluate_expr(stmt->stmt_if.if_clause->cond, context);
    Result ret = Result();
    uint64_t* match_cond = cond_result.get<uint64_t>();
    if(!match_cond) {
        printf("GOT: %s\n", cond_result.to_string().c_str());
        return Result("expected integer in the if condition");
    }
    if(*match_cond){
        // Process the 'if' block        
        for (Stmt* s : stmt->stmt_if.if_clause->block) {
            if(s->kind == STMT_RETURN) return prep_stmt_return(s, context);
            else {
                ret = prep_stmt(s, context);
            }
        }        
        return ret;
    }        
    // Process any 'elif' clauses
    bool elif_match = false;
    for (IfClause* elif : stmt->stmt_if.elif_clauses) {
        Result elif_cond = evaluate_expr(elif->cond, context);            
        match_cond = elif_cond.get<uint64_t>();
        if(!match_cond){
            return Result("expected integer in the if condition");
        }
        
        for (Stmt* s : elif->block) {
            if(s->kind == STMT_RETURN) return prep_stmt_return(s, context);
            
            else {
                ret = prep_stmt(s, context);
            }
        }
        if (*match_cond) {                
            return ret;
        }
    }
    // Process the 'else' block
    
    for (Stmt* s : stmt->stmt_if.else_block) {
        if(s->kind == STMT_RETURN) return prep_stmt_return(s, context);
            
        else {
            ret = prep_stmt(s, context);
        }
    }
    return ret;
    
}
// Preprocess the 'switch' statement
Result prep_stmt_switch(Stmt* stmt, CTContext& context) {
    Result cond_result = evaluate_expr(stmt->stmt_switch.cond, context);
    for (SwitchCase* case_item : stmt->stmt_switch.cases) {
        // Process each case pattern
        for (SwitchCasePattern* pattern : case_item->patterns) {
            // Example of processing a pattern (you can adapt it based on your use case)
            // Depending on the pattern type, you can handle different cases
        }
        // Process the statements in each case block
        for (Stmt* case_stmt : case_item->block) {
            prep_stmt(case_stmt, context);
        }
    }
    // Handle the default case if necessary
    if (stmt->stmt_switch.has_default) {
        for (Stmt* default_stmt : stmt->stmt_switch.default_case) {
            prep_stmt(default_stmt, context);
        }
    }
    return Result();
}
// Preprocess the 'for' statement
Result prep_stmt_for(Stmt* stmt, CTContext& context) {
    // Process the 'for' initialization expression
    evaluate_expr(stmt->stmt_for.init, context);
    // Process the 'for' condition expression
    Result cond_result = evaluate_expr(stmt->stmt_for.cond, context);
    // Process the 'for' increment expression
    evaluate_expr(stmt->stmt_for.inc, context);
    // Process the 'for' block
    for (Stmt* block_stmt : stmt->stmt_for.block) {
        prep_stmt(block_stmt, context);
    }
    return Result();
}
// Preprocess the 'while' statement
Result prep_stmt_while(Stmt* stmt, CTContext& context) {
    for(;;){
        // Process the 'while' condition expression
        Result cond_result = evaluate_expr(stmt->stmt_while->cond, context);
        uint64_t* cond_vp = cond_result.get<uint64_t>();
        if(!cond_vp){
            printf("[GOT]: %s\n", cond_result.to_string().c_str());
            return Result("while condition expectd to be a number");
        }
        if(!*cond_vp) break;
        // Process the 'while' block
        for (Stmt* block_stmt : stmt->stmt_while->block) {
            if(block_stmt->kind == STMT_RETURN) return prep_stmt_return(block_stmt, context);            
            prep_stmt(block_stmt, context);
        }
    }
    return Result();
}
// Preprocess the 'return' statement
Result prep_stmt_return(Stmt* stmt, CTContext& context) {
    // Process the return expression
    return evaluate_expr(stmt->expr, context);
    // Handle return value processing (storing or using the return value as needed)
}
// General handler for statement preprocessing
Result prep_stmt(Stmt* stmt, CTContext& context) {
    switch (stmt->kind) {
        case StmtKind::STMT_EXPR:       return prep_stmt_expr(stmt, context);                
        case StmtKind::STMT_IF:         return prep_stmt_if(stmt, context);                
        case StmtKind::STMT_SWITCH:     return prep_stmt_switch(stmt, context);                
        case StmtKind::STMT_FOR:        return prep_stmt_for(stmt, context);                
        case StmtKind::STMT_WHILE:      return prep_stmt_while(stmt, context);                
        case StmtKind::STMT_RETURN:     assert(0 && "this section should not be handled in here");
        default: return Result("Failed to preprocess statement, unreachable");  
    }
}
Result prep_block(SVec<Stmt*> block, CTContext& context){        
    Result result;
    for(Stmt* stmt: block){
        if(stmt->kind == STMT_RETURN) return prep_stmt_return(stmt, context);
        result = prep_stmt(stmt, theContext);            
        
    }        
    if(!result.is_success()) {
        result = Result(CTNone());
    }
    return result;
}
Result prep_decl(Decl* decl, CTContext& context) {
    // Dispatch based on declaration kind
    switch (decl->kind) {
        case DECL_VAR:          return handle_var_decl(decl, context);
        case DECL_PROC:         return handle_proc_decl(decl, context);
        case DECL_AGGREGATE:    return handle_aggregate_decl(decl, context);
        case DECL_ENUM:         return handle_enum_decl(decl, context);
        case DECL_TYPE:         return handle_type_alias_decl(decl, context);
        case DECL_USE:          return handle_use_decl(decl, context);
        case DECL_IMPL:         return handle_impl_decl(decl, context);
        case DECL_CONSTEXPR:
        case DECL_NONE: // Not supported
        default:            return Result("Error: Unsupported or invalid declaration kind.");
    }
}
// Handle variable declarations
Result handle_var_decl(Decl* decl, CTContext& context) {
    if (!decl->var.type) {
        return Result("Error: Variable declaration missing type.");
    }
    if(context.get_variable_ptr(std::string(decl->name))){
        return Result("Error: Variable '" + std::string(decl->name) + "' already declared.");
    }
    
    // Evaluate initializer expression, if present
    if (decl->var.init) {
        Result initializer_result = evaluate_expr(decl->var.init, context);
        if (!initializer_result.is_success()) {
            return Result("Error: Failed to evaluate initializer for variable '" + std::string(decl->name) + "'.");
        }
        context.add_variable(std::string(decl->name), initializer_result);
    }
    else {
        context.add_variable(std::string(decl->name), Result(0));
    }
    return Result("Success: Variable '" + std::string(decl->name) + "' declared.");
}
// Handle procedure declarations
Result handle_proc_decl(Decl* decl, CTContext& context) {        
    if(context.get_variable_ptr(std::string(decl->name))){
        return Result("Error: Procedure '" + std::string(decl->name) + "' already declared.");
    }        
    context.add_variable(std::string(decl->name), Result(decl));
    return Result("Success: Procedure '" + std::string(decl->name) + "' declared.");
}
// Handle aggregate (struct/union) declarations
Result handle_aggregate_decl(Decl* decl, CTContext& context) {
    // Just declare the structure
    context.add_variable(decl->name, Result(decl));
    return Result();
}
// Handle enumeration declarations
Result handle_enum_decl(Decl* decl, CTContext& context) {
    uint64_t iota = 0;
    // declare the enumeration
    context.add_variable(decl->name, Result(decl));
    for(EnumItem* item: decl->enumeration.items){
        assert(!item->init);
        context.add_variable(item->name, Result(iota++));
    }    
    return Result();
}
// Handle type alias declarations
Result handle_type_alias_decl(Decl* decl, CTContext& context) {
    printf("unimplemented %s\n", __func__);
    return Result("Success: Type alias '" + std::string(decl->name) + "' declared.");
}
// Handle 'use' declarations (imports)
Result handle_use_decl(Decl* decl, CTContext& context) {
    printf("unimplemented %s\n", __func__);
    return Result("Success: 'Use' declaration processed.");
}
// Handle implementation declarations
Result handle_impl_decl(Decl* decl, CTContext& context) {
    Result* target = context.get_variable_ptr(std::string(decl->impl.target));
    if(!target) {        
        return Result("Error: Target '" + std::string(decl->impl.target) + "' not found for implementation.");
    }
    if(Decl** decl_ptr = target->get<Decl*>()){
        Decl* d = *decl_ptr;
        for(Decl* impl_decl : decl->impl.body){            
            impl_decl->name = format_impl_name(d->name, impl_decl->name);
            context.add_variable(impl_decl->name, Result(impl_decl));            
        }                
    }        
    return Result();
}
Result prep_run_ast(std::vector<Decl*> ast) {        
    for(Decl* node: ast){
        Result result = prep_decl(node, theContext);
        if(!result.is_success()){
            printf("[ERROR]: %s\n", result.to_string().c_str());
            return Result("error ast node");
        }            
    }
    // context is ready, now find main procedure 
    Result* main_procedure = theContext.get_variable_ptr("main");
    if(!main_procedure) {
        theContext.print_all();
        printf("Expected main procedure.\n");
        return Result("main procedure not defined");
    }
    Decl** decl_ptr = main_procedure->get<Decl*>();
    
    if(!decl_ptr){
        printf("[ERROR]: overwritte main, expected it to be a procedure\n");
        return Result("bad main");            
    }
    Decl* decl = *decl_ptr;
    std::vector<Result> main_args;
    return evaluate_decl_call(decl, main_args, theContext);        
}
Result print_func(std::vector<Result> args, CTContext& context){
    // Check if arguments are provided            
    if (args.empty()) {
        return Result("Error: No arguments provided to print.");
    }
    // Iterate over arguments and print their values
    for (Result arg : args) {
        // Assuming Expr can be converted to Result somehow (simplified for this example)            
        printf("%s", arg.to_string().c_str());
    }
    printf("\n");
    // Return Success Result
    return Result();
}
Result pietra_str_cast(std::vector<Result> args, CTContext& context) {
    // Check if exactly one argument is provided
    if (args.size() != 1) {
        return Result("Error: pietra_str_cast expects exactly one argument");
        }

        // Evaluate the argument expression
        Result result = args.at(0);

    // Check if the evaluation was successful
    if (!result.is_success()) {
        return Result("Error: Failed to evaluate argument for pietra_str_cast");
    }
    // Return the string representation of the result        
    return Result(result.to_string());
}
Result pietra_int_cast(std::vector<Result> args, CTContext& context) {
    // Check if exactly one argument is provided
    if (args.size() != 1) {
        return Result("Error: pietra_int_cast expects exactly one argument");
    }
    // Evaluate the argument expression
    Result result = args.at(0);
    // Check if the evaluation was successful
    if (!result.is_success()) {
        return Result("Error: Failed to evaluate argument for pietra_int_cast");
    }
    // Try to cast the result to an integer
    if (auto str_val = result.get<std::string>()) {
        try {
            // Attempt to convert the string to an integer
            return Result(std::stoul(*str_val));  // Convert string to integer
        } catch (const std::invalid_argument& e) {
            return Result("Error: Invalid string for integer conversion");
        } catch (const std::out_of_range& e) {
            return Result("Error: Integer out of range");
        }
    } else if (auto int_val = result.get<uint64_t>()) {
        return Result(*int_val);  // Return the integer value directly if it's already an integer
    } else if (auto double_val = result.get<double>()) {
        return Result(static_cast<uint64_t>(*double_val));  // Convert float/double to int
    }
    // If casting fails for other types, return an error
    return Result("Error: Cannot cast the value to an integer");
}
std::vector<Result> CT_buffer;          
// Append a result to the CT_buffer
Result pietra_buffer_append(std::vector<Result> args, CTContext& context) {
    // Expect exactly one argument
    if (args.size() != 1) {
        return Result("Error: pietra_buffer_append expects exactly one argument");
    }
    // Evaluate the expression
    Result result = args.at(0);
    // If evaluation fails, return an error
    if (!result.is_success()) {
        return Result("Error: Failed to evaluate argument for pietra_buffer_append");
    }
    // Append the result to the buffer
    CT_buffer.push_back(result);
    return Result();
}
// Clear the CT_buffer
Result pietra_buffer_clear(std::vector<Result> args, CTContext& context) {
    // Expect no arguments
    if (args.size() != 0) {
        return Result("Error: pietra_buffer_clear expects no arguments");
    }
    // Clear the buffer
    CT_buffer.clear();
    return Result();
}
// Get the size of the CT_buffer
Result pietra_buffer_size(std::vector<Result> args, CTContext& context) {
    // Expect no arguments
    if (args.size() != 0) {
        return Result("Error: pietra_buffer_size expects no arguments");
    }
    // Return the size of the buffer        
    return Result(static_cast<uint64_t>(CT_buffer.size()));
}
Result pietra_make_array(std::vector<Result> args, CTContext& context){    
    CTArray* array = new CTArray();
    return Result(array);
}
// Get the value at a specific index from the CT_buffer
Result pietra_buffer_get(std::vector<Result> args, CTContext& context) {
    // Expect exactly one argument (index)
    if (args.size() != 1) {
        return Result("Error: pietra_buffer_get expects exactly one argument");
    }
    // Evaluate the index argument
    Result index_result = args.at(0);
    printf("GET INDEX: %s\n", index_result.to_string().c_str());
    if (!index_result.is_success()) {
        return Result("Error: Failed to evaluate index argument for pietra_buffer_get");
    }
    // Try to get the index as an integer
    uint64_t* index = index_result.get<uint64_t>();
    
    if (!index) {
        return Result("Error: Invalid index argument for pietra_buffer_get");
    }
    
    // Check if the index is within bounds
    if (*index >= CT_buffer.size()) {
        return Result("Error: Index out of bounds for pietra_buffer_get");
    }
    // Return the value at the given index
    return CT_buffer[*index];
}
void add_function_to_context(std::string func_name, CTFunction::FuncType func){
    CTFunction* func_allocation = new CTFunction(func_name, func);
    theContext.add_variable(func_name, Result(func_allocation));
}

void init_prep(){        
    theContext.add_variable("true", Result(Value((uint64_t)true)));
    theContext.add_variable("false", Result(Value((uint64_t)false)));
    theContext.add_variable("None", Result(CTNone()));
    
    // Create a CTFunction for print and add it to the context
    add_function_to_context("print", print_func);    
    add_function_to_context("make_array", pietra_make_array);
    add_function_to_context("str", pietra_str_cast);
    add_function_to_context("int", pietra_int_cast);
    add_function_to_context("buffer_append", pietra_buffer_append);        
    add_function_to_context("buffer_clear", pietra_buffer_clear);
    add_function_to_context("buffer_size", pietra_buffer_size);
    add_function_to_context("buffer_get", pietra_buffer_get);
}

#endif /*PREPROCESS_CPP*/