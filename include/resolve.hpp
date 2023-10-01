#ifndef RESOLVE_HPP
#define RESOLVE_HPP
#include "ast.hpp"
#include "type.hpp"

using namespace Pietra::Ast;

namespace Pietra::Resolver {
    
    enum SymState {
        SYM_UNRESOLVED,
        SYM_RESOLVING,
        SYM_RESOLVED
    };
    enum SymKind{
        SYM_NONE        = 0,
        SYM_VAR,
        SYM_CONST,
        SYM_PROC,
        SYM_AGGREGATE,
        SYM_TYPE,
        SYM_ENUM_CONST,
        SYM_CONSTEXPR,
        SYM_TYPE_ALIAS
    };
    union Val {
        bool b;
        char c;
        unsigned char uc;
        signed char sc;
        short s;
        unsigned short us;
        int i;
        unsigned u;
        long l;
        unsigned long ul;
        long long ll;
        unsigned long long ull;
        uintptr_t p;
    };

    struct Sym{
        SymState    state;    
        SymKind     kind;
        const char* name;
        Decl*       decl;            
        Type*       type;
        Val         val;
    };
    

    struct Operand {
        Type*   type;
        bool    is_lvalue;
        bool    is_const;
        Val     val;
    };

    struct ResolvedExpr {
        Expr* expr;
        Type* type;
    };
    /* 
        returns the resolved ast
    */
    SVec<Decl*> resolve_ast(SVec<Decl*> ast);
    void declare_decl(Decl* decl);
    Sym* sym_from_decl(Decl* decl);    
    void init_builtin_syms();
    void sym_push(Sym* sym);
    void resolved_sym_push(Sym* sym);
    Sym* sym_get(const char* name);
    Sym* new_sym_decl(SymKind kind, const char* name, Decl*& decl);
    Type* resolve_expr(Expr* expr);
    void resolve_stmt(Stmt* stmt);
    void resolve_stmt_block(SVec<Stmt*> block);
    Type* resolve_typespec(TypeSpec*& type);
    Type* type_from_typespec(TypeSpec* typespec);
    void resolve_decl_var(Decl* dvar);
    void resolve_decl_aggregate(Decl* decl);
    void resolve_decl_constexpr(Decl* decl);
    void resolve_decl_proc(Decl* decl);
    void resolve_decl(Decl* decl);
    void resolve_sym(Sym* sym);
    Sym* resolve_name(const char* name);
    void resolve_init_var(const char* name, TypeSpec* typespec, Expr*& init);
}
#endif /*RESOLVE_HPP*/