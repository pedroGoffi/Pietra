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
        return nullptr;
    }
    Sym* sym_new(const char* name, Decl* decl){
        assert(decl);
        assert(!sym_get(decl->name));

        Sym* sym    = arena_alloc<Sym>();
        sym->decl   = decl;
        sym->name   = Core::cstr(name);
        sym->state  = SYM_UNRESOLVED;
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
                "syscall"
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
        resolve_typespec(type);

        if(init){
            *init = *PreprocessExpr::expr(init);
            resolve_expr(init);
        }

        if(isLocal){
            CBridge::CVar* var = CBridge::CreateVar(
                name,
                type->resolvedTy,
                init,
                false,
                isParam
            );
        }
        
        return operand_rvalue(type->resolvedTy);
    }
    Operand resolve_name(const char* name){
        Sym* sym = sym_get(name);
        assert(sym);

        resolve_sym(sym);                
        return operand_rvalue(sym->type);        
    }
    Operand resolve_call(Expr* base, SVec<Expr*> args){
        Operand bop = resolve_expr(base);
        assert(bop.type);
        printf("ok got %s\n", bop.type->repr());
        exit(1);
    }
    Operand resolve_expr(Expr* expr){        
        switch(expr->kind){
            case EXPR_INT:      return operand_lvalue(type_int(64), {.ul = expr->int_lit});
            case EXPR_STRING:   return operand_lvalue(type_string(), {});
            case EXPR_NAME:     return resolve_name(expr->name);
            case EXPR_CALL:     return resolve_call(expr->call.base, expr->call.args);            
            case EXPR_INIT_VAR: return resolve_var_init(expr->init_var.name, expr->init_var.type, expr->init_var.rhs, true, false);
        }
        pPrint::expr(expr);
        exit(1);
    }

    void resolve_stmt(Stmt* &stmt){
        switch(stmt->kind){
            case Ast::STMT_EXPR:
                resolve_expr(stmt->expr);
                break;
            default: assert(0);
        }
    }

    void resolve_stmt_block(SVec<Stmt*> block){
        for(Stmt* stmt: block) 
            resolve_stmt(stmt);
    }

    void resolve_decl_var(Decl* &d){
        if(!d->var.type){                           
            d->var.type = Utils::typespec_name("any");            
        }
        resolve_typespec(d->var.type);

        if(d->var.init){
            *d->var.init = *PreprocessExpr::expr(d->var.init);
            Operand op = resolve_expr(d->var.init);
            

            assert(d->var.type);
            assert(d->var.type->resolvedTy);

            if(!d->var.type->resolvedTy->typeCheck(op.type)){
                printf("[ERR]: trying to assign the variable %s: %s to %s\n", 
                    d->name,
                    d->var.type->resolvedTy->repr(),
                    op.type->repr()
                );
                exit(1);
            }
            
        }        
    }
    void resolve_decl_proc(Decl* &d){
        assert(d->kind == DECL_PROC);
        Type* type = resolve_typespec(d->proc.ret);
        CBridge::CProc* proc = CBridge::CreateProc(d->name, type);

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
            case DECL_NONE:                    
            case DECL_AGGREGATE:
            case DECL_ENUM:
            case DECL_USE:
            case DECL_TYPE:
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
        resolve_decl(sym->decl);
        sym->state = SYM_RESOLVED;
        final_ast_push(sym->decl);
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