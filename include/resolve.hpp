#ifndef RESOLVE_HPP
#define RESOLVE_HPP
#include "ast.hpp"
#include "packages.hpp"
#include "type.hpp"
#include "bridge.hpp"

using namespace Pietra::Ast;

namespace Pietra::Resolver {
    struct Sym;    

    enum SymState {
        SYM_UNRESOLVED,
        SYM_RESOLVING,
        SYM_RESOLVED
    };
    enum SymKind{
        SYM_NONE,
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
    
    
    
    struct SymImpl {
        Sym*           self;
        SVec<Sym*>     body;        

        void push(Sym* sym);
        Sym* find(const char* name);
    };
    // SYM -> struct 
    // SYM has IMPLS 
    struct Sym{
        SymState        state;    
        SymKind         kind;
        const char*     name;
        Decl*           decl;            
        Expr*           expr;
        Type*           type;
        Val             val;        
        
        SymImpl         impls;
    };
    

    struct Operand {
        Type*   type;
        bool    is_lvalue;
        bool    is_const;
        Val     val;
    };
    Operand operand_rvalue(Type* type);
    Operand operand_lvalue(Type* type, Val val);
    
    Sym* sym_constexpr_int(Expr* e);
    Operand resolve_expr(Expr* expr);
    Operand resolve_name(const char* name);
    Type* resolve_typespec(TypeSpec* &ts);
    
    Operand resolve_var_init(const char* &name, TypeSpec* &type, Expr* &init, bool isLocal, bool isParam);
    void resolve_stmt(Stmt* &stmt);
    void resolve_stmt_block(SVec<Stmt*> block);
    void resolve_decl_var(Decl*& var);
    void resolve_decl(Decl* &decl);
    Sym* resolve_sym(Sym* sym);
    void resolve_decl_proc(Decl* &d);    
    SVec<Decl*> resolve_package(PPackage* &package);
}
#endif /*RESOLVE_HPP*/