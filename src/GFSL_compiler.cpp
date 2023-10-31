#ifndef PIETRA_TO_GFSL
#define PIETRA_TO_GFSL

#include "../include/GFSL_compiler.hpp"
#include "ast.cpp"
#include "resolve.cpp"
#include <cassert>
#include <map>
#include <random>
#include <string>

using namespace Pietra;
using namespace Pietra::Ast;


namespace Pietra::GFSLCompiler{
    CContext pctx;

    void compile_store(Type* base){
        switch(base->size){
            case 1:
                GFSL::make_WRITE8();
                break;
            case 4:
                GFSL::make_WRITE32();
                break;

            case 8:
            default:
                GFSL::make_WRITE64();
                break;
        }
    }
    Type* compile_load(Type* type){        
        switch(type->kind){            
            case Ast::TYPE_I8:
                GFSL::make_LOAD8();
                break;

            case Ast::TYPE_PTR: 
            case Ast::TYPE_I64:
            default:
                GFSL::make_LOAD64();
                break;
        }

        return type;
    }
    
    Type* opt_load(Type* type){                
        if(pctx.expect(C_GET_ADDR)){
            return type_ptr(type);
        }
        
        return compile_load(type);                
    }
    Type* compile_unary(Lexer::tokenKind kind, Expr* e){
        switch(kind){
            case Lexer::TK_AMPERSAND:   {
                pctx.set(C_GET_ADDR);
                return compile_expr(e);                
            }

            case Lexer::TK_SUB: {
                Type* ty = compile_expr(e);
                compile_expr(Utils::expr_int(-1));
                GFSL::make_MULT();
                return ty;
            }

            case Lexer::TK_MULT:    {
                Type* ty = compile_expr(e);
                
                return compile_load(ty->base);
            }

            case Lexer::TK_NOT: {
                Type* ty = compile_expr(e);
                GFSL::make_NOT();
                return ty;
            }
            default: assert(0);
        }
    }
    Type* compile_binary(Lexer::tokenKind kind, Expr* left, Expr* right){
        switch(kind){
            case Lexer::TK_EQ:  {
                compile_expr(right);

                pctx.set(C_GET_ADDR);
                Type* lhs_t = compile_expr(left);
                compile_store(lhs_t);
                return type_void();
            }

            case Lexer::TK_ADD: {
                compile_expr(right);
                Type* lhs = compile_expr(left);                
                GFSL::make_PLUS();
                return lhs;
                
            }

            case Lexer::TK_LT:  {
                compile_expr(left);
                compile_expr(right);
                GFSL::make_LTHAN();
                return type_int(8);
                
            }
            case Lexer::TK_CMPEQ:   {
                compile_expr(left);
                compile_expr(right);
                GFSL::make_EQUALS();
                return type_int(8); // Boolean
                
            }

            default: assert(0);
        }
    }
    CVar* compile_var_proto(std::string name, Type* type, bool isGlobal, Expr* init){
        GFSL::Variable* gfvar = new GFSL::Variable(GFSL::type_addr, {GFSL::TK_NAME, name}, type->size);
        
        return new CVar{
            .name = name, 
            .type = type, 
            .isGlobal = isGlobal,
            .init = init,
            .gfid = 0,
            .gfvar = gfvar
        };
    }
    Type* compile_init_var(std::string name, Type* type, Expr *init, bool isGlobal){
        CVar* cvar = compile_var_proto(name, type, isGlobal, init);

        if(isGlobal){
            pctx.install_global(cvar);
            return type_void();            
        }
        else {
            CProc* cp = pctx.cp;
            assert(cp);

            
            cp->install_local(cvar);

            if(init){
                Type* rhs = compile_expr(init);
                GFSL::make_PUSH_LOCAL_MEM(GFSL::ctx.cp, cvar->gfvar->stack_offset);
                compile_store(rhs);
            }
            return type_void();            
        }
    }
    Type* compile_call(Expr* base, SVec<Expr*> args){
        for(Expr* arg: args) compile_expr(arg);

        if(base->kind == EXPR_NAME){
            if(base->name == Core::cstr("syscall")){
                GFSL::make_SYSCALL(args.len());
                return type_int(64);
            }
            else if(base->name == Core::cstr("dump")){
                GFSL::make_DUMP();
                return type_int(64);
            }
            else if(base->name == Core::cstr("__builtin_get_buffer")){
                if(args.len() != 1){
                    printf("[ERR]: __builtin_get_buffer expects 1 argument.\n");
                    exit(1);
                }
                GFSL::make_PUSH_PTR();
                GFSL::make_PLUS();
                return type_ptr(type_void());
            }
        }

        return compile_expr(base);
    }
    Type* compile_name(const char* name){        
        std::string str = std::string(name);        
        // Check for procedures
        if(CProc* cp = pctx.find_proc(str)){            
            GFSL::make_CALL_PROC(cp->gfid);
            return cp->ret;
        }        

        assert(pctx.cp);
        // Check for parameters
        if(CVar* param = pctx.cp->find_param(str)){
            GFSL::make_PUSH_LOCAL_MEM(pctx.cp->gfproc, param->gfvar->stack_offset);
            
            return opt_load(param->type);
        }
        // Check for locals
        if(CVar* local = pctx.cp->find_local(str)){
            GFSL::make_PUSH_LOCAL_MEM(pctx.cp->gfproc, local->gfvar->stack_offset);
            return opt_load(local->type);
        }
        // Check for globals
        for(CVar* global: pctx.globals){
            if(global->name == str){
                GFSL::make_PUSH_GLOBAL(global->gfid);
                if(pctx.expect(C_GET_ADDR)){
                    return type_ptr(global->type);
                }
                return compile_load(global->type);
            }
        }



        printf("[ERR]: the name '%s' is not declared in this scope.\n", name);
        exit(1);
    }
    Type* compile_expr(Expr* e){
        switch(e->kind){
            case EXPR_INT:
                GFSL::make_PUSH_INT(e->int_lit);
                return type_int(64);
            case EXPR_STRING:                
                GFSL::make_PUSH_CSTR(std::string(e->string_lit));
                return type_string();
            case EXPR_NAME:
                return compile_name(e->name);
            case EXPR_UNARY:
                return compile_unary(e->unary.unary_kind, e->unary.expr);
            case EXPR_BINARY:
                return compile_binary(e->binary.binary_kind, e->binary.left, e->binary.right);
            case EXPR_INIT_VAR:
                return compile_init_var(
                    std::string(e->init_var.name),
                    resolve_typespec(e->init_var.type),
                    e->init_var.rhs,
                    false
                );
            case EXPR_CALL: 
                return compile_call(e->call.base, e->call.args);

            case EXPR_ARRAY_INDEX:  {
                // ptr+index*8
                
                Type* base = compile_expr(e->array.base);                                
                { // index * sizeof(ptr[0])
                    compile_expr(e->array.index);
                    compile_expr(Utils::expr_int(base->size));
                    GFSL::make_MULT();
                }                
                GFSL::make_PLUS(); // ptr + index * sizeof(ptr[0])
                compile_load(base->base);

                
                return base;
            }
            default: 
                pPrint::expr(e);
                assert(0);
        }
    }

    void compile_stmt(Stmt* stmt){
        switch(stmt->kind){
            case Ast::STMT_EXPR:    {
                Type* type = compile_expr(stmt->expr);
                if(type != type_void()){
                    GFSL::make_DROP();                    
                }
            } break;

            case Ast::STMT_WHILE:   {
                size_t begin_addr = GFSL::program_size();
                compile_expr(stmt->stmt_while->cond);
                GFSL::Inst* jz = GFSL::make_JMP_IF(0);
                compile_block(stmt->stmt_while->block);
                GFSL::make_JMP(begin_addr);
                jz->op = GFSL::program_size();
            } break;
            case Ast::STMT_RETURN:  {
                compile_expr(stmt->expr);
                GFSL::make_PROC_RETURN();
                
            } break;

            default: assert(0);
        }
    }
    void compile_block(SVec<Stmt*> block){
        for(Stmt* stmt: block) compile_stmt(stmt);
    }
    void compile_cproc_params(CProc* cp, SVec<ProcParam*> params){
        assert(cp);
        for(ProcParam* param: params){
            CVar* cvar = compile_var_proto(
                std::string(param->name), 
                resolve_typespec(param->type),
                false,
                param->init);            

            cp->install_param(cvar);
        }
    }
    void compile_decl_proc(Decl* d){
        CProc* cp = CProc::Init(std::string(d->name));
        pctx.install_proc(cp);

        compile_cproc_params(cp, d->proc.params);
        if(d->proc.ret){
            cp->ret = resolve_typespec(d->proc.ret);
        }
        else {
            cp->ret = type_void();
        }

        GFSL::make_PROC_ENTRY();
        if(cp->name == "main"){
            for(CVar* global: pctx.globals){
                // Initialize global vars inside "main" procedure
                if(global->init){
                    compile_expr(global->init);
                    GFSL::make_PUSH_GLOBAL(global->gfid);
                    compile_store(global->type);
                }
            }
        }
        compile_block(d->proc.block);
        GFSL::make_PROC_LEAVE();

    }

    void compile_decl(Decl* decl){        
        switch(decl->kind){
            case DECL_PROC: 
                compile_decl_proc(decl); 
                break;
            case DECL_VAR:
                compile_init_var(
                    decl->name,
                    resolve_typespec(decl->var.type),
                    decl->var.init,
                    true
                );
                break;
            default: assert(0);
        }
    }

    void compile_ast(SVec<Decl*> ast){
        for(Decl* decl: ast){
            compile_decl(decl);
        }
        
        GFSL::compileProgram("build/gfsl_translation");
    }
    
}
#endif /*PIETRA_TO_GFSL*/