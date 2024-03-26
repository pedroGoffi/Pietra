#ifndef PIETRA_PREP_CPP
#define PIETRA_PREP_CPP

#include "../include/preprocess.hpp"
#include "../include/ast.hpp"
#include "../include/resolve.hpp"

#include "bridge.cpp"
#include "file.cpp"
#include "ast.cpp"
#include <cstdint>
#include <random>

using namespace Pietra;
using namespace Pietra::Ast;
using namespace Pietra::Preprocess;

// Buffer is a bridge to the preprocessed world to the compiled world
struct Buffer {    
    Sym* sym = sym_var("Buffer", type_aggregate({}), nullptr);
    Buffer(){
        this->sym->decl = Utils::decl_aggregate("Buffer", Pietra::Ast::AGG_STRUCT, {});
    }
    int find_index_of(TypeField* tf){        
        for(int i = 0; i < this->sym->decl->aggregate.items.len();){
            for(const char* name: this->sym->decl->aggregate.items.at(i)->field.names){
                if(tf->name == name) return i;
                i++;
            }
        }

        return -1;
    }

    AggregateItem* get_agg_item_at(int index){
        return this->sym->decl->aggregate.items.at(index);
    }
    TypeField* find(const char* name){
        for(TypeField* tf: this->sym->type->aggregate.items){
            if(tf->name == name) return tf;
        }
        return nullptr;
    }
    void push(TypeField* tf, Expr* value){        
        assert(!this->find(tf->name));

        // Create the type item
        this->sym->type->aggregate.size += tf->type->calcSize();                
        this->sym->type->aggregate.items.push(tf);        
        

        // Create the decl item
        SVec<const char*> name; name.push(tf->name);                
        AggregateItem* item = Utils::aggregate_item_field(name, Utils::typespec_name("null", {}), value);        
        this->sym->decl->aggregate.items.push(item);        
    }
};

Buffer buffer;

Expr* PreprocessExpr::binary_add(Expr* lhs, Expr* rhs){
    lhs = expr(lhs);
    rhs = expr(rhs);
    if(lhs->kind == rhs->kind){
        
        switch(lhs->kind){
            case Ast::EXPR_INT: {
                lhs->int_lit += rhs->int_lit;
                return lhs;
            }
            case Ast::EXPR_FLOAT:   {
                lhs->float_lit += rhs->float_lit;
                
                return lhs;
            }
            case Ast::EXPR_STRING:                
                lhs->string_lit = Core::cstr((std::string(lhs->string_lit) + std::string(rhs->string_lit)).data());
                return lhs;            
        }
    }
    return nullptr;    
}
Expr* PreprocessExpr::binary_sub(Expr* lhs, Expr* rhs){
    lhs = expr(lhs);
    rhs = expr(rhs);
    if(lhs->kind == rhs->kind){
        
        switch(lhs->kind){
            case Ast::EXPR_INT: {
                lhs->int_lit -= rhs->int_lit;                
                return lhs;
            }
            case Ast::EXPR_FLOAT:   {
                lhs->float_lit -= rhs->float_lit;
                
                return lhs;
            }            
        }
    }    
    return nullptr; 
}
Expr* PreprocessExpr::binary_mult(Expr* lhs, Expr* rhs){
    lhs = expr(lhs);
    rhs = expr(rhs);
    if(lhs->kind == rhs->kind){
        
        switch(lhs->kind){
            case Ast::EXPR_INT: {
                lhs->int_lit *= rhs->int_lit;                
                return lhs;
            }
            case Ast::EXPR_FLOAT:   {
                lhs->float_lit *= rhs->float_lit;
                
                return lhs;
            }            
        }
    }    
    return nullptr; 
}        

Expr* PreprocessExpr::binary(Lexer::tokenKind kind, Expr *lhs, Expr *rhs){
    switch(kind){
        case Lexer::TK_ADD:     return binary_add(lhs, rhs);
        case Lexer::TK_SUB:     return binary_sub(lhs, rhs);
        case Lexer::TK_MULT:    return binary_mult(lhs, rhs);        
    }
    return nullptr;
}
Expr* PreprocessExpr::init_var(Expr* e){
    if(e->init_var.rhs){        
        *e->init_var.rhs = *(PreprocessExpr::expr(e->init_var.rhs)?: e);
    }
    return e;
}
Expr* PreprocessExpr::expr(Expr* e){
    assert(e and "ERR: prep_expr with nullptr expr");

    switch(e->kind){
        
        case ExprKind::EXPR_INT:        return e;
        case ExprKind::EXPR_FLOAT:      return e;
        case ExprKind::EXPR_STRING:     return e;        

        case ExprKind::EXPR_BINARY:            
            return binary(
                e->binary.binary_kind, 
                e->binary.left,
                e->binary.right)    
            ?: e;

        case ExprKind::EXPR_INIT_VAR:   return init_var(e);        
        default: return e;

    }
}

namespace Pietra::Preprocess{
    using namespace Resolver;
    const char*     prep_proc_puts          = Core::cstr("puts");
    const char*     prep_proc_readFile      = Core::cstr("readFile");
    const char*     prep_proc_quit          = Core::cstr("quit");
    const char*     prep_proc_BufferPush    = Core::cstr("BufferPush");
    
    FILE* prep_file = stdout;
    PreprocessContext context;

    template<typename T> void expect_arity(SVec<T>& list, int expect, const char* err_msg){
        if(list.len() != expect){
            printf("[ERROR]: %s", err_msg);
            exit(1);
        }
    }

    Operand eval_call(Expr* base, SVec<Expr*> args, PreprocessContext& ctx){
        assert(base->kind == EXPR_NAME);
        if(base->name == prep_proc_puts){
            expect_arity(args, 1, "prep-proc `puts` wrong usage, proc trace: puts(cstr): null\n");
            Expr* arg = args.at(0);
            Operand op = eval_expr(arg, ctx);
            fprintf(prep_file, "%s", (char*) op.val.p);
            return op;            
        }

        else if(base->name == prep_proc_readFile){            
            expect_arity(args, 1, "prep-proc `readFile` wrong usage, proc trace readFile(filename: cstr): cstr.\n");            

            Operand op = eval_expr(args.at(0), ctx);
            if(op.type != type_string()){
                printf("[ERROR]: the 0-nth argument on fopen must be cstr.\n");
                exit(1);
            }                        
            
            
            const char* filename    = (const char*) op.val.p;             
            const char* intern_str = Core::cstr(fileReader::read_file(filename));
            
            return operand_lvalue(type_any(), {.p = (uintptr_t) intern_str});
        }
        else if(base->name == prep_proc_quit){
            expect_arity(args, 1, "prep-proc `quit` wrong usage, proc trace quit(exit_code: i64): null.\n");
            Operand op = eval_expr(args.at(0), ctx);            
            exit(op.val.i);
        }
        else if(base->name == prep_proc_BufferPush){
            expect_arity(args, 2, "prep-proc `BufferPush` wrong usage, proc trace BufferPush(field, value: Expression): null.\n");            
            Expr* field = args.at(0);
            Expr* value = args.at(1);
            assert(field->kind == EXPR_NAME);

            Operand value_op = eval_expr(value, ctx);

            if(not value_op.is_lvalue){
                printf("[ERROR]: `BufferPush` wrong usage, expected lvalue.\n");
                exit(1);
            }

            TypeField* tf = Utils::init_typefield(field->name, type_void());            
            Expr* e;
            // TODO: Move this section to anoter function
            
            ////////////////////////////////////////////////////
            if(value->kind == Ast::EXPR_INT){
                e = Utils::expr_int(value_op.val.i);
            }
            else if(value->kind == Ast::EXPR_STRING){
                e = Utils::expr_string((const char*) value_op.val.p);
            }            
            else {
                printf("[ERROR]: `BufferPush` wrong usage. idk what happened.\n");
                exit(1);
            }               
            ////////////////////////////////////////////////////
            buffer.push(tf, e);         
            return operand_lvalue(type_void(), {0});
        }

        else {
            printf("[ERROR]: the prep-proc %s was not declared in this scope.\n", base->name);
            exit(1);
        }
    }
    Operand eval_eq(Expr* var, Expr* right, PreprocessContext& ctx){
        if(CBridge::CVar* cvar = ctx.find_gvar(var)){
            cvar->init = right;
            return eval_expr(cvar->init, ctx);
        }
        else {
            // Create the var
            assert(var->kind == Ast::EXPR_NAME);
            const char* name    = var->name;
            Operand r_op        = resolve_expr(right);
            Type* type          = r_op.type;

            CBridge::CVar* var = CBridge::CreateVar(name, type, right, true, false);
            ctx.install_gvar(var);

            Sym* sym = sym_var(var->name, var->type, right);            
            return r_op;
        }
    }
    Operand eval_bin(Lexer::tokenKind kind, Expr* l, Expr* r, PreprocessContext& ctx){
        if(kind == Lexer::TK_EQ){
            return eval_eq(l, r, ctx);
        }

        printf("[ERROR]: preprocess wasn't meant to preprocess this kind.\n");
        exit(1);
    }
    Operand eval_name(const char* name, PreprocessContext& ctx){
        if(CBridge::CVar* gvar = ctx.find_gvar(name)){
            return eval_expr(gvar->init, ctx);
        }

        else {
            printf("[ERROR]: idk what is %s\n", name);
            exit(1);
        }
    }
    Operand eval_expr(Expr* e, PreprocessContext& ctx){        
        switch(e->kind){
            case Ast::EXPR_INT:     return operand_lvalue(type_int(64, false), {.ul = e->int_lit});
            case Ast::EXPR_STRING:  return operand_lvalue(type_string(), {.p = (uintptr_t)e->string_lit});
            case Ast::EXPR_CALL:    return eval_call(e->call.base, e->call.args, ctx);
            case Ast::EXPR_BINARY:  return eval_bin(e->binary.binary_kind, e->binary.left, e->binary.right, ctx);
            case Ast::EXPR_NAME:    
                if(e->name == Core::cstr("true")){
                    return operand_lvalue(type_int(64, false), {true});
                }
                if(e->name == Core::cstr("false")){
                    return operand_lvalue(type_int(64, false), {false});
                }
                
                return eval_name(e->name, ctx);
            default:
                printf("[ERROR]: Cant preprocess this kind.\n");
                exit(1);
        }
    }
    void eval_node(Stmt* node, PreprocessContext& ctx){
        switch(node->kind){
            case Ast::STMT_EXPR:
                eval_expr(node->expr, ctx);
                break;

            case Ast::STMT_IF: {
                Operand op = eval_expr(node->stmt_if.if_clause->cond, ctx);
                bool run_node = false;

                if(op.is_lvalue) run_node = op.val.i;
                else {
                    printf("idk, if error i guess\n");
                    exit(1);
                }


                if(run_node){
                    eval_block(node->stmt_if.if_clause->block);
                    return;
                }
                
                break;                
            }
            default:
                printf("[ERROR]: Cant preprocess this statement.\n");
                exit(1);
        }
    }

    void eval_block(SVec<Stmt*> block){        
        for(Stmt* node: block){
            eval_node(node, context);
        }
        for(CBridge::CVar* var: context.gvars){
            var->init = 0;
        }
    }
}

#endif /*PIETRA_PREP_CPP*/