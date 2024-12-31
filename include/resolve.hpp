#ifndef RESOLVE_HPP
#define RESOLVE_HPP
#include "ast.hpp"
#include "packages.hpp"
#include "type.hpp"


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
        SYM_ENUM,
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
        bool    m_isNone = false;
        
        bool isNone(){ return this->m_isNone; }
    };
    Operand operand_rvalue(Type* type);
    Operand operand_lvalue(Type* type, Val val);
    
    Sym* sym_constexpr_int(Expr* e);
    Operand resolve_expr(Expr* &expr);
    Operand resolve_name(const char* name);
    Type* resolve_typespec(TypeSpec* &ts);
    
    Operand resolve_var_init(const char* &name, TypeSpec* &type, Expr* &init, bool isLocal, bool isParam);
    void resolve_stmt(Stmt* &stmt);
    void resolve_stmt_block(SVec<Stmt*> block);
    void resolve_decl_var(Decl*& var);
    void resolve_decl(Decl* &decl, Type* type);
    Sym* resolve_sym(Sym* sym);
    void resolve_decl_use(Decl* &d);
    void resolve_decl_proc(Decl* &d, Type* type);
    SVec<Decl*> resolve_package(PPackage* &package);
    Sym* sym_new(const char* name, Decl* decl);
    Sym* sym_var(const char* name, Type* type, Expr* expr);
 
    void resolve_impl(const char* target, SVec<Decl*> body);    
    Operand create_lambda_callee(SVec<ProcParam*> params, TypeSpec* ret, SVec<Stmt*> block, std::string*);
    Operand resolve_lambda(Expr*& expr, std::string*);


    class Scope {
public:
    std::vector<Resolver::Sym*> m_symbols; ///< List of symbols in the current scope.
    int m_lvl; ///< Current scope level.
    int scope_level(); ///< Returns the current scope level.
    /**
     * @brief Adds a symbol to the current scope.
     * @param sym Pointer to the symbol to be added.
     */
    void add_sym(Resolver::Sym* sym);

    /**
     * @brief Finds a symbol by its name in the current scope.
     * @param name Name of the symbol to find.
     * @return Pointer to the found symbol, or nullptr if not found.
     */
    Resolver::Sym* find_sym(const char* name);
};

/**
 * @class ScopeManager
 * @brief Manages multiple scopes and provides functionality to enter new scopes and manage symbols.
 */
class ScopeManager {
public:
    std::vector<Scope*> m_scopes; ///< List of all scopes managed by the ScopeManager.
    int m_lvl = 0; ///< Current scope level.
    int get_scope_level(); ///< Returns the current scope level.

    /**
     * @brief Enters a new scope.
     */
    void enter_scope();
    void leave_scope();
    void print_all();

    /**
     * @brief Adds a symbol to the current scope.
     * @param sym Pointer to the symbol to be added.
     * @return Index of the added symbol in the current scope.
     */
    int add_sym(Resolver::Sym* sym);

    /**
     * @brief Finds a symbol by its name across all scopes.
     * @param name Name of the symbol to find.
     * @return Pointer to the found symbol, or nullptr if not found.
     */
    Resolver::Sym* find_sym(const char* name);
};
}


#endif /*RESOLVE_HPP*/