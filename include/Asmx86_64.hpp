#ifndef ASMPLANG_HPP
#define ASMPLANG_HPP
#include "arena.hpp"
#include "interns.hpp"
#include "resolve.hpp"
#include "type.hpp"
#include <map>
#include "../src/interns.cpp"
#include "bridge.hpp"
#include "ast.hpp"

using namespace Pietra;
using namespace Pietra::Resolver;
using namespace Pietra::CBridge;

namespace Pietra::Asm {    
    void push(const char* what, Type* type);
    Type* pop(const char* target);
    Type* cmp_zero(Type* ty);
                
    void store();
    void store(Type* lhs, Type* rhs);
    
    void compile_init_var(const char* name, TypeSpec* type, Expr* init, bool is_global, State::CompilerState state);
    void opt_load();
    const char* get_word_size(int size);
    Type* compile_expr(Expr* expr, State::CompilerState& state);
        Type* compile_lea(Variables::Variable* local);
        Type* compile_binary(TokenKind kind, Ast::Expr* lhs, Ast::Expr* rhs, State::CompilerState& state);
        Type* compile_modify_var(Variables::Variable* var, Ast::Expr* expr);
        void compile_cmp_zero(Type* type);
        Type* compile_logic_expr(TokenKind kind, Expr* lhs, Expr* rhs);
        Type* compile_assign_var_expr(Expr* var_expr, Expr* rhs);
        void compile_pre_call_args(SVec<Expr*> args);
    Type* load(Type* type);
    void compile_stmt(Stmt* stmt);    
    void compile_block(SVec<Stmt*> block);
    void compile_decl(Decl* decl);        
        void compile_decl_proc(Decl* decl);
            void compile_proc_params(Procedures::Procedure* cp);

    enum COMPILER_TARGET {CT_LINUX, CT_WINDOWS};    
    void compile_ast(SVec<Decl*> ast, COMPILER_TARGET target, const char* output_file, bool verbose);
    void compile_segment_data();
    void compile_segment_bss();
    
}



#endif /*ASMPLANG_HPP*/