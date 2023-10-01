#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include "ast.hpp"

namespace Pietra::Parser{
    using namespace Ast;    
    Expr* literal_expr_assign(const char* name);
    Expr* literal_expr();
    Expr* base_expr();
    Expr* unary_expr();
    Expr* mult_expr();
    Expr* add_expr();
    Expr* cmp_expr();
    Expr* logic_expr();
    Expr* cast_expr();
    Expr* ternary_expr();
    Expr* expr();
    SVec<Expr*> expr_list();
    TypeSpec* typespec_base();
    TypeSpec* typespec();

    
    SVec<Stmt*> stmt_opt_curly_block();   
     
    Stmt* stmt_if();
        IfClause* stmt_if_clause();
    
    Stmt* stmt_for();
    Stmt* stmt_while();

    Stmt* stmt_switch();
        SwitchCase* switch_case();
        SVec<SwitchCasePattern*> switch_case_patterns();

    Stmt* stmt();


    Decl* decl();
        Decl* decl_use();
            SVec<const char*> use_module_names();
            SVec<const char*> use_names();
        Decl* decl_type();
        Decl* decl_base(const char* name, SVec<Note*> notes);
            Decl* decl_constexpr(const char* name);
            Decl* decl_proc(const char* name);
                SVec<ProcParam*> proc_params();
                ProcParam* proc_param();
            Decl* decl_struct(const char* name);            
                Decl* decl_enum(const char* name);
                Decl* decl_aggregate(const char* name, aggregateKind kind);
                Decl* decl_union(const char* name);
                    AggregateItem* aggregate_item();
    
};
#endif /*PARSER_HPP*/