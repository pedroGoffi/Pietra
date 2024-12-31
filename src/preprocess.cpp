#ifndef PREPROCESS_CPP
#define PREPROCESS_CPP
#include "../include/smallVec.hpp"
#include "../include/ast.hpp"
#include "preprocessresult.cpp"

namespace Pietra::Preprocess {    
    using namespace Ast;
    Result evaluate_int_expr(Expr* expr);
    Result evaluate_float_expr(Expr* expr);
    Result evaluate_string_expr(Expr* expr);
    Result evaluate_name_expr(Expr* expr, CTContext& context);
    Result evaluate_reassing(Expr* expr, CTContext& context);
    Result evaluate_binary_expr(Expr* expr, CTContext& context);
    Result evaluate_call_expr(Expr* expr, CTContext& context);
    Result evaluate_init_var_expr(Expr* expr, CTContext& context);
    Result evaluate_binary_lesss(Result lhs, Result rhs, CTContext& context);
    Result evaluate_binary_plus(Result lhs, Result rhs, CTContext& context);
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


    Result evaluate_decl_call(Decl* decl, std::vector<Expr*> args, CTContext& context);
    Result prep_decl(Decl* decl, CTContext& context);
    Result handle_var_decl(Decl* decl, CTContext& context);
    Result handle_proc_decl(Decl* decl, CTContext& context);
    Result handle_aggregate_decl(Decl* decl, CTContext& context);
    Result handle_enum_decl(Decl* decl, CTContext& context);
    Result handle_type_alias_decl(Decl* decl, CTContext& context);
    Result handle_use_decl(Decl* decl, CTContext& context);
    Result handle_impl_decl(Decl* decl, CTContext& context);
    Result run_ast(std::vector<Decl*> ast);
    

    CTContext theContext;

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
    return context.get_variable(expr->name);  // Retrieve variable from context
}

Result evaluate_reassing(Expr* expr, CTContext& context){
    Expr* lhs   = expr->binary.left;
    Result rhs  = evaluate_expr(expr->binary.right, context);

    if(lhs->kind == EXPR_NAME) {
        context.add_variable(lhs->name, rhs);
        return rhs;
    }
    else {
        return Result("Failed to reassign");
    }
    
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

    // If types do not match or cannot perform the operation, return an error
    return Result("Error: Invalid types for binary < operation");
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
        return Result("Error: failed to evaluate left operand");
    }
    // Evaluate right operand
    Result rhs = evaluate_expr(expr->binary.right, context);
    
    if (!rhs.is_success()) {
        return Result("Error: failed to evaluate right operand");
    }
    TokenKind op = expr->binary.binary_kind;    
    if(op == TK_plus)   return evaluate_binary_plus(lhs, rhs, context);
    if(op == TK_less)   return evaluate_binary_lesss(lhs, rhs, context);
    else {
        return Result("Error: unknown operator");
    }
}
Result evaluate_decl_call(Decl* decl, std::vector<Expr*> args, CTContext& context){
    printf("CALLING : %s\n", decl->name);
    assert(decl->kind == DECL_PROC);
    // TODO: manage context
    // setup parameters
    // enter the block 
    return prep_block(decl->proc.block, context);
}
Result evaluate_call_expr(Expr* expr, CTContext& context) {    
    Result base = evaluate_expr(expr->call.base, context);
    if(!base.is_success()) return Result("Error: failed to call function");
    
    std::vector<Expr*> args;
    for(Expr* arg: expr->call.args) args.push_back(arg);

    if(CTFunction** func_ptr = base.get<CTFunction*>()){
        CTFunction* func = *func_ptr;
        assert(func);
        
        return func->call(args, context);
    } 

    if(Decl** decl_ptr = base.get<Decl*>()){
        Decl* decl = *decl_ptr;
        assert(decl->kind == DECL_PROC);


        return evaluate_decl_call(decl, args, context);    
    }

    else {
        return Result("BAD FUNCTION DEFINITION");
    }
    return Result();
}
Result evaluate_init_var_expr(Expr* expr, CTContext& context) {
    Result rhs_value = evaluate_expr(expr->init_var.rhs, context);
    context.add_variable(std::string(expr->init_var.name), rhs_value);  // Store the initialized variable in context
    return rhs_value;
}

// Generic function to evaluate an expression
Result evaluate_expr(Expr* expr, CTContext& context) {
    switch (expr->kind) {
        case ExprKind::EXPR_INT:        return evaluate_int_expr(expr);
        case ExprKind::EXPR_FLOAT:      return evaluate_float_expr(expr);
        case ExprKind::EXPR_STRING:     return evaluate_string_expr(expr);
        case ExprKind::EXPR_NAME:       return evaluate_name_expr(expr, context);
        case ExprKind::EXPR_INIT_VAR:   return evaluate_init_var_expr(expr, context);
        case ExprKind::EXPR_CALL:       return evaluate_call_expr(expr, context);
        case ExprKind::EXPR_BINARY:     return evaluate_binary_expr(expr, context);
        // Add additional cases here, using helper functions for each kind
        default: return Result("Error: Unsupported expression type");
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
        uint64_t* match_cond = cond_result.get<uint64_t>();
        if(!match_cond) {
            return Result("expected integer in the if condition");
        }

        if(*match_cond){
            // Process the 'if' block        
            for (Stmt* s : stmt->stmt_if.if_clause->block) {
                prep_stmt(s, context);
            }
            return Result();
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
                prep_stmt(s, context);
            }

            if (*match_cond) return Result();
        }
        // Process the 'else' block
        for (Stmt* s : stmt->stmt_if.else_block) {
            prep_stmt(s, context);
        }
        return Result();
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
            case StmtKind::STMT_RETURN:     return prep_stmt_return(stmt, context);                
            default: return Result("Failed to preprocess statement, unreachable");  
        }
    }

    Result prep_block(SVec<Stmt*> block, CTContext& context){        
        for(Stmt* stmt: block){
            if(stmt->kind == STMT_RETURN) return prep_stmt_return(stmt, context);

            Result result = prep_stmt(stmt, theContext);            
            if(!result.is_success()){
                printf("[PREPROCESS ERROR]: %s\n", result.to_string().c_str());                
            }
        }        
        return Result();
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
        printf("PREP: adding procedure: %s\n", decl->name);            
        context.add_variable(std::string(decl->name), Result(decl));
        return Result("Success: Procedure '" + std::string(decl->name) + "' declared.");
    }

    // Handle aggregate (struct/union) declarations
    Result handle_aggregate_decl(Decl* decl, CTContext& context) {
        printf("unimplemented %s\n", __func__);
        return Result("Success: Aggregate '" + std::string(decl->name) + "' declared.");
    }

    // Handle enumeration declarations
    Result handle_enum_decl(Decl* decl, CTContext& context) {
        printf("unimplemented %s\n", __func__);
        return Result("Success: Enumeration '" + std::string(decl->name) + "' declared.");
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
        printf("unimplemented %s\n", __func__);
        return Result("Success: Implementation '" + std::string(decl->name) + "' processed.");
    }

    Result run_ast(std::vector<Decl*> ast) {        
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
            printf("Expected main procedure.\n");
            return Result("main procedure not defined");
        }

        Decl** decl_ptr = main_procedure->get<Decl*>();
        
        if(!decl_ptr){
            printf("[ERROR]: overwritte main, expected it to be a procedure\n");
            return Result("bad main");            
        }

        Decl* decl = *decl_ptr;
        std::vector<Expr*> main_args;
        return evaluate_decl_call(decl, main_args, theContext);        
    }

    Result print_func(std::vector<Expr*> args, CTContext& context){
        // Check if arguments are provided            
        if (args.empty()) {
            return Result("Error: No arguments provided to print.");
        }
        // Iterate over arguments and print their values
        for (Expr* arg : args) {
            // Assuming Expr can be converted to Result somehow (simplified for this example)
            Result res = evaluate_expr(arg, context);
            printf("%s", res.to_string().c_str());
        }
        printf("\n");
        // Return Success Result
        return Result();
    }
    Result pietra_str_cast(std::vector<Expr*> args, CTContext& context) {
        // Check if exactly one argument is provided
        if (args.size() != 1) {
            return Result("Error: pietra_str_cast expects exactly one argument");
        }

        // Evaluate the argument expression
        Result result = evaluate_expr(args[0], context);

        // Check if the evaluation was successful
        if (!result.is_success()) {
            return Result("Error: Failed to evaluate argument for pietra_str_cast");
        }

        // Return the string representation of the result        
        return Result(result.to_string());
    }

    Result pietra_int_cast(std::vector<Expr*> args, CTContext& context) {
        // Check if exactly one argument is provided
        if (args.size() != 1) {
            return Result("Error: pietra_int_cast expects exactly one argument");
        }

        // Evaluate the argument expression
        Result result = evaluate_expr(args[0], context);

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
    Result pietra_buffer_append(std::vector<Expr*> args, CTContext& context) {
        // Expect exactly one argument
        if (args.size() != 1) {
            return Result("Error: pietra_buffer_append expects exactly one argument");
        }

        // Evaluate the expression
        Result result = evaluate_expr(args[0], context);

        // If evaluation fails, return an error
        if (!result.is_success()) {
            return Result("Error: Failed to evaluate argument for pietra_buffer_append");
        }

        // Append the result to the buffer
        CT_buffer.push_back(result);

        return Result();
    }

    // Clear the CT_buffer
    Result pietra_buffer_clear(std::vector<Expr*> args, CTContext& context) {
        // Expect no arguments
        if (args.size() != 0) {
            return Result("Error: pietra_buffer_clear expects no arguments");
        }

        // Clear the buffer
        CT_buffer.clear();

        return Result();
    }

    // Get the size of the CT_buffer
    Result pietra_buffer_size(std::vector<Expr*> args, CTContext& context) {
        // Expect no arguments
        if (args.size() != 0) {
            return Result("Error: pietra_buffer_size expects no arguments");
        }

        // Return the size of the buffer
        return Result(static_cast<uint64_t>(CT_buffer.size()));
    }

    // Get the value at a specific index from the CT_buffer
    Result pietra_buffer_get(std::vector<Expr*> args, CTContext& context) {
        // Expect exactly one argument (index)
        if (args.size() != 1) {
            return Result("Error: pietra_buffer_get expects exactly one argument");
        }

        // Evaluate the index argument
        Result index_result = evaluate_expr(args[0], context);
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

        
        // Create a CTFunction for print and add it to the context
        add_function_to_context("print", print_func);
        add_function_to_context("str", pietra_str_cast);
        add_function_to_context("int", pietra_int_cast);
        add_function_to_context("buffer_append", pietra_buffer_append);        
        add_function_to_context("buffer_clear", pietra_buffer_clear);
        add_function_to_context("buffer_size", pietra_buffer_size);
        add_function_to_context("buffer_get", pietra_buffer_get);
    }
}
#endif /*PREPROCESS_CPP*/