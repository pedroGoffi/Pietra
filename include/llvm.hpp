#ifndef  LLVM_COMPILER_HPP
#define  LLVM_COMPILER_HPP

#include "arena.hpp"
#include "ast.hpp"
#include "resolve.hpp"
#include "smallVec.hpp"
#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
//#include <llvm/TargetParser/Host.h>
#include <llvm/Transforms/Scalar.h>
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>


enum LCErr {
    ERR_NONE,    
};
namespace Pietra::LLVMCompiler {
    using namespace llvm;    
    
    struct C_struct {        
        const char*             name;
        SVec<AggregateItem*>    items;
        llvm::StructType*       type;

        static C_struct* Create(
            const char*             name, 
            SVec<AggregateItem*>    items,
            llvm::StructType*       type)
            {
                C_struct* st    = arena_alloc<C_struct>();
                st->name        = name;
                st->items       = items;
                st->type        = type;
                return st;
            }
    };
    struct C_var {
        const char* name;
        Value*      var;
        Value*      init;

        static C_var* New(const char* name, Value* var, Value* init){
            C_var* c_var  = arena_alloc<C_var>();
            c_var->name   = name;
            c_var->var    = var;
            c_var->init   = init;
            return c_var;
        }
    };
    enum UCtx_state {
        U_NONE,
        U_GET_ADDR
    };

    struct UCtx {
    private:
        UCtx_state state = U_NONE;
    public:
        void set(UCtx_state state){
            this->state = state;
        }
        void reset(){ this->state = U_NONE; }

        UCtx_state get(){
            UCtx_state state = this->state;
            this->state = U_NONE;
            return state;
        }        
        
        bool expect(UCtx_state test){
            bool val = this->is(test);
            this->reset();
            return val;
        }
        bool is(UCtx_state test){
            return this->state == test;
        }
    };
    BasicBlock* new_bb(Function* curr_F);
    Value* compile_icmp_zero(Value* v);
    Value* cond_jmp(Value* cond, BasicBlock* True, BasicBlock* False);
    Value* get_null();
    Value* compile_expr(Expr* expr);
        Value* compile_field_access(Expr* parent, Expr* field);
        Value* compile_unary(Lexer::tokenKind kind, Expr* expr);
        Value* compile_binary(Lexer::tokenKind kind, Expr* lhs, Expr* rhs);
        Value* compile_init_var(const char* name, TypeSpec* pts, Expr* rhs, bool isGlobal);
        Value* compile_call(Expr* base, SVec<Expr*> pargs);
        Value* compile_modify(Lexer::tokenKind kind, Expr* var, Expr* rhs);
    Value* compile_stmt(Stmt* stmt);
        Value* compile_if(IfClause* ifc, SVec<IfClause*> elifs, SVec<Stmt*> else_block);
            Value* compile_if_clause(IfClause* clause, BasicBlock* block, BasicBlock* abs_end);
        Value* compile_for(Expr* init, Expr* cond, Expr* inc, SVec<Stmt*> block);
        Value* compile_while(Expr* cond, SVec<Stmt*> block);
    Value* compile_stmt_block(SVec<Stmt*> block);
    
    void compile__start();
    LCErr compile_decl(Decl* decl);        
        llvm::Function* compile_decl_proc(Decl* decl);                
            llvm::Function* proc_proto(const char* &name, SVec<ProcParam*> &params, TypeSpec* &ret);
        LCErr compile_decl_aggregate(Decl* decl);
            llvm::StructType* compile_struct(const char* name, SVec<AggregateItem*> items);
    LCErr compile_ast(Core::SVec<Decl*> sym);

    llvm::Type* type(Ast::Type* ptype);
    llvm::Type* typespec(TypeSpec* ts);
}

#endif /*LLVM_COMPILER_HPP*/