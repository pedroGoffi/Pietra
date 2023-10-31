/*
The resolver will
1]: order the AST
2]: TODO: type checking
3]: TODO: const evaluation    
*/
#ifndef RESOLVE_CPP
#define RESOLVE_CPP
#include "../include/resolve.hpp"
#include "../include/packages.hpp"
#include "../include/bridge.hpp"
#include "ast.cpp"
#include "pprint.cpp"    
#include "bridge.cpp"
#include "preprocess.cpp"

namespace Pietra::Resolver{
    SVec<Sym*> Syms;
    SVec<Sym*> localSyms;
    SVec<Decl*> final_ast;

    void final_ast_push(Decl* d){
        final_ast.push(d);
    }
    Operand operand_lvalue(Type *type, Val val){
        return {
            .type = type,
            .is_lvalue = true,
            .val = val                        
        };
    }
    Operand operand_rvalue(Type *type){
        return {
            .type = type,
            .is_lvalue = false,                                    
        };
    }
    Sym* sym_get(const char* name){
        for(auto& sym: Syms){
            if(sym->name == name) return sym;
        }
        for(auto& sym: localSyms){
            if(sym->name == name) return sym;                            
        }
        return nullptr;
    }
    Sym* sym_var(const char* name, Type* type, Expr* expr){
        Sym* s = arena_alloc<Sym>();
        s->name = name;
        s->type = type;
        s->expr = expr;
        return s;
    }
    Sym* sym_new(const char* name, Decl* decl){
        assert(decl);
        assert(!sym_get(decl->name));
        Sym* sym    = arena_alloc<Sym>();
        sym->decl   = decl;
        sym->name   = Core::cstr(name);
        sym->state  = SYM_UNRESOLVED;
        sym->type   = type_unresolved();

        switch(decl->kind){
            case Ast::DECL_VAR:
                sym->type = resolve_typespec(decl->var.type);
                break;
            case Ast::DECL_TYPE:    {
                const char* name    = decl->name;
                Type* type          = resolve_typespec(decl->type_alias.type);
                CBridge::type_set(name, type);
                sym->type = type_void();
                break;
            }
            case DECL_PROC:
                Type* proc;
                
                SVec<TypeField*>    params;
                for(ProcParam* param: decl->proc.params){
                    Type* ty = resolve_typespec(param->type);
                    params.push(Utils::init_typefield(param->name, ty));
                }
                Type*               ret = resolve_typespec(decl->proc.ret);
                
                sym->type = type_proc(params, ret, decl->proc.is_vararg);

                break;
        }
        return sym;
    }

    void declare_package(PPackage* package){
        for(Decl* decl: package->ast){
            Sym* sym = sym_new(decl->name, decl);
            Syms.push(sym);
        }
    }

    #define DEFINE_BUILTIN(_name, ...)                      \
        {                                                   \
            Syms.push(sym_new(_name, __VA_ARGS__));         \
        }

    void declare_built_in(){

        DEFINE_BUILTIN("true",
            Utils::decl_var("true", 
                Utils::typespec_name("i64"),
                Utils::expr_int(1)));

        DEFINE_BUILTIN("false",
            Utils::decl_var("false", 
                Utils::typespec_name("i64"),
                Utils::expr_int(0)));

        DEFINE_BUILTIN("null",
            Utils::decl_var("null", 
                Utils::typespec_name("i64"),
                Utils::expr_int(0)));
        {
            const char* builtin_procs[] = {
                "dump",
                "syscall",
                "asm"
            };
            for(auto& str: builtin_procs){
                DEFINE_BUILTIN(str,
                    Utils::decl_proc(
                        str,
                        {},
                        Utils::typespec_name("any"),
                        {},
                        false,
                        true,
                        {},
                        true                                                
                    ));
            }
        }
    }
    #undef DEFINE_BUILTIN

    TypeSpec* decay_typespec(TypeSpec* ts){    
        if(ts->kind == Pietra::Ast::TYPESPEC_CONST or ts->kind == Pietra::Ast::TYPESPEC_POINTER){
            return ts->base;
        }    
        return ts;
    }

    Type* resolve_typespec(TypeSpec* &ts){    
        if(!ts){
            ts = Utils::typespec_name("any");
            return resolve_typespec(ts);
        }

        switch(ts->kind){
            case TYPESPEC_NAME:
            #define TS_CHECK_TYPE(__ty)                     \
                if(ts->name == (__ty())->name){             \
                    ts->resolvedTy = __ty();                \
                    break;                                  \
                }               

            #define TS_CHECK_INT(__size)                        \
                if(ts->name == type_int(__size)->name) {        \
                    ts->resolvedTy = type_int(__size);          \
                    break;                                      \
                }

            
                TS_CHECK_INT(8);
                TS_CHECK_INT(16);
                TS_CHECK_INT(32);
                TS_CHECK_INT(64);

                TS_CHECK_TYPE(type_void);
                TS_CHECK_TYPE(type_any);
                
                if(Type* alias = CBridge::type_get(ts->name)){
                    ts->resolvedTy = alias;
                    break;
                }

                printf("Undefined type: %s \n", ts->name);
                exit(1);


            #undef CHECK_INT
            case TYPESPEC_POINTER:  {
                Type* base = resolve_typespec(ts->base);
                ts->resolvedTy = type_ptr(base);
                break;
            }    
            case TYPESPEC_NONE:        

            case TYPESPEC_ARRAY:
            case TYPESPEC_PROC:
            case TYPESPEC_CONST:
            case TYPESPEC_TEMPLATE:
            default: assert(0);
        }
        assert(ts->resolvedTy);
        return ts->resolvedTy;
    }
    Operand resolve_var_init(const char* &name, TypeSpec* &type, Expr* &init, bool isLocal, bool isParam){        
        Type* ty = resolve_typespec(type);

        if(init){
            *init = *PreprocessExpr::expr(init);
            Operand rhs = resolve_expr(init);

            if(!ty->typeCheck(rhs.type)){
                printf("[ERROR]: tring to assign the %s variable (%s:%s) to the type %s.\n",
                    isParam? "parameter": isLocal? "local":"global",
                    name,
                    ty->repr(),
                    rhs.type->repr()
                );
                exit(1);
            }            
        }        

        
        
        if(isLocal or isParam){            
            Sym* s = sym_var(name, type->resolvedTy, init);            
            assert(s);
            localSyms.push(s);
            CBridge::CreateVar(
                name,
                type->resolvedTy,
                init,
                false,
                isParam
            );
        
        }
        else {
            CBridge::CreateVar(
                name,
                type->resolvedTy,
                init,
                true,
                false
            );
        }
        return operand_rvalue(type->resolvedTy);
    }
    Operand resolve_name(const char* name){
        Sym* sym = sym_get(name);
        if(!sym){
            printf("[ERROR]: the name '%s' is not declared in this scope.\n", name);
            exit(1);
        }

        resolve_sym(sym);                
        return operand_rvalue(sym->type);        
    }
    Operand resolve_call(Expr* base, SVec<Expr*> args){
        Operand bop = resolve_expr(base);
        assert(bop.type);
        if(!bop.type->isCallable()){
            printf("[ERROR]: trying to call a non-callable object %s\n", bop.type->repr());
            exit(1);
        }
        int args_sz     = args.len();
        int params_sz   = bop.type->proc.params.len();
        if(args_sz != params_sz and not bop.type->proc.is_vararg){
            printf("[ERROR]: trying to call the procedure (%s) that expects %i parameters but got %i arguments.\n",
                bop.type->repr(),
                params_sz,
                args_sz
            );
            exit(1);
        }

        for(Expr* arg: args) resolve_expr(arg);

        Type* ret = bop.type->proc.ret_type;
        if(bop.type->kind == TYPE_PTR){
            return operand_rvalue(ret);
        }
        else {
            return operand_lvalue(ret, {});
        }
    }
    Operand resolve_unary(Lexer::tokenKind kind, Expr* expr){
        switch(kind){
            case Lexer::TK_AMPERSAND:   {
                Operand op = resolve_expr(expr);
                if(op.is_lvalue){
                    printf("[ERROR]: trying to get the address if an lvalue.\n");
                    exit(1);
                }

                op.type = type_ptr(op.type);
                return op;
            }

            case Lexer::TK_MULT:    {
                // derrefencing
                Operand op = resolve_expr(expr);
                if(op.is_lvalue){
                    printf("[ERROR: trying to derreference an lvalue.\n");
                    exit(1);
                }
                if(op.type->kind != TYPE_PTR){
                    printf("[ERROR]: trying to derreference a non-pointer type: %s\n", op.type->repr());
                    exit(1);
                }

                op.type = op.type->base;
                return op;
            }
            default: assert(0);
        }
    }
    static inline bool is_assign(Lexer::tokenKind kind){
        return kind == Lexer::TK_EQ;
    }
    Operand resolve_assign(Expr* base, Expr* rhs){
        Operand b = resolve_expr(base);
        Operand r = resolve_expr(rhs);

        if(!b.type->typeCheck(r.type)){
            printf("[ERROR]: trying to assign a type %s to %s\n",
                b.type->repr(),
                r.type->repr());
            exit(1);
        }
    }
    Operand resolve_binary(Lexer::tokenKind kind, Expr* lhs, Expr* rhs){
        if(is_assign(kind)){
            return resolve_assign(lhs, rhs);
        }
        switch(kind){
            case Lexer::TK_CMPEQ:   {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                return operand_lvalue(type_int(8), {});                
            }
            case Lexer::TK_SUB: {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                if(l.type->kind == r.type->kind and l.type->kind == TYPE_PTR){
                    return operand_lvalue(type_int(64), {});
                }

                return l;
            }
            case Lexer::TK_ADD: {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                return l;
            }
            default: assert(0);
        }
    }
    Operand resolve_index(Expr* &expr){
        Expr* base  = expr->array.base;
        Expr* index = expr->array.index;

        //*index = *PreprocessExpr::expr(index);

        // base[index] -> *(base + sizeof(base)+index)
        Operand bop = resolve_expr(base);
        if(bop.type->kind != TYPE_PTR){
            printf("[ERROR]: trying to get the index of a non-pointer object.\n");
            exit(1);
        }
        Type* b_t = bop.type->base;
        
        expr = Utils::expr_unary(Lexer::TK_MULT, expr); 
        
        Expr* offset = Utils::expr_binary(Lexer::TK_MULT, index, Utils::expr_int(b_t->size));
        Expr* item = Utils::expr_binary(Lexer::TK_ADD, base, offset);

        expr = item; 
        
        if(b_t->kind == TYPE_PTR){            
            return operand_rvalue(b_t);
        }
        else {
            return operand_lvalue(b_t, {});
        }

    }
    Operand resolve_expr(Expr* expr){        
        switch(expr->kind){
            case EXPR_INT:          return operand_lvalue(type_int(64), {.ul = expr->int_lit});
            case EXPR_STRING:       return operand_lvalue(type_string(), {});
            case EXPR_NAME:         return resolve_name(expr->name);
            case EXPR_CALL:         return resolve_call(expr->call.base, expr->call.args);            
            case EXPR_INIT_VAR:     return resolve_var_init(expr->init_var.name, expr->init_var.type, expr->init_var.rhs, true, false);
            case EXPR_UNARY:        return resolve_unary(expr->unary.unary_kind, expr->unary.expr);
            case EXPR_BINARY:       return resolve_binary(expr->binary.binary_kind, expr->binary.left, expr->binary.right);
            case EXPR_ARRAY_INDEX:  return resolve_index(expr);
        }
        pPrint::expr(expr);
        exit(1);
    }

    void resolve_stmt(Stmt* &stmt){
        switch(stmt->kind){
            case Ast::STMT_EXPR:
                resolve_expr(stmt->expr);
                break;
            case Ast::STMT_IF:
                resolve_expr(stmt->stmt_if.if_clause->cond);
                resolve_stmt_block(stmt->stmt_if.if_clause->block);
                break;
            case Ast::STMT_WHILE:   {
                resolve_expr(stmt->stmt_while->cond);
                resolve_stmt_block(stmt->stmt_while->block);
                break;
            }
            case Ast::STMT_RETURN: {
                Operand got         = resolve_expr(stmt->expr);
                Type*   expected    = CBridge::ctx.Cp->type;
                
                if(!expected->typeCheck(got.type)){
                    printf("[ERROR]: expected %s in the return of the procedure %s, but got: %s\n",
                        expected->repr(),
                        CBridge::ctx.Cp->name,
                        got.type->repr()
                    );
                    exit(1);
                }

                break;
            }
            default: assert(0);
        }
    }

    void resolve_stmt_block(SVec<Stmt*> block){
        for(Stmt* stmt: block) 
            resolve_stmt(stmt);
    }

    void resolve_decl_var(Decl* &d){
        resolve_var_init(d->name, d->var.type, d->var.init, false, false);        
    }
    void resolve_decl_proc(Decl* &d){
        assert(d->kind == DECL_PROC);
        localSyms.free();
        Type* type = resolve_typespec(d->proc.ret);
        CBridge::CProc* proc = CBridge::CreateProc(d->name, type);
        

        for(ProcParam* pp: d->proc.params){
            resolve_var_init(pp->name, pp->type, pp->init, false, true);
        }

        resolve_stmt_block(d->proc.block);
    }
    void resolve_decl(Decl* &decl){
        switch(decl->kind){
            case DECL_VAR:
                resolve_decl_var(decl);
                break;
            case DECL_PROC:
                resolve_decl_proc(decl);
                break;
            case DECL_TYPE:
            case DECL_NONE:                    
            case DECL_AGGREGATE:
            case DECL_ENUM:
            case DECL_USE:
            
            case DECL_CONSTEXPR:
                printf("[ERR]: Resolving unimplemented for:\n");
                pPrint::decl(decl);                        
        }

    }
    void resolve_sym(Sym* sym){
        assert(sym);
        if(sym->state == SYM_RESOLVED) return;
        if(sym->state == SYM_RESOLVING){
            printf("Cyclic dependence at: %s.\n", sym->name);
            exit(1);
        }

        sym->state = SYM_RESOLVED;
        if(sym->decl){ 
            // some syms are expressions
            resolve_decl(sym->decl);
        }
        sym->state = SYM_RESOLVED;

        if(sym->decl and sym->decl->kind != Ast::DECL_TYPE){            
            final_ast_push(sym->decl);
        }
        
    }
    SVec<Decl*> resolve_package(PPackage* &package){
        declare_built_in();
        declare_package(package);

        for(Sym* sym: Syms){
            resolve_sym(sym);
        }    
        return final_ast;
    }
}
#endif /*RESOLVE_CPP*/