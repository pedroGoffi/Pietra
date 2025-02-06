#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include "ast.hpp"

namespace Pietra::Parser{
    using namespace Ast;    
    Expr* literal_expr_assign(Lexer* lexer, const char* name);
    Expr* literal_expr(Lexer* lexer);
    Expr* base_expr(Lexer* lexer);
    Expr* unary_expr(Lexer* lexer);
    Expr* mult_expr(Lexer* lexer);
    Expr* add_expr(Lexer* lexer);
    Expr* cmp_expr(Lexer* lexer);
    Expr* logic_expr(Lexer* lexer);
    Expr* assign_expr(Lexer* lexer);
    Expr* cast_expr(Lexer* lexer);
    Expr* ternary_expr(Lexer* lexer);
    Expr* cast_expr(Lexer* lexer);
    Expr* expr(Lexer* lexer);
    SVec<Expr*> expr_list(Lexer* lexer);
    TypeSpec* typespec_base(Lexer* lexer);
    TypeSpec* typespec(Lexer* lexer);

    
    SVec<Stmt*> stmt_opt_curly_block(Lexer* lexer);   
     
    Stmt* stmt_if(Lexer* lexer);
        IfClause* stmt_if_clause(Lexer* lexer);
    
    Stmt* stmt_for(Lexer* lexer);
    Stmt* stmt_while(Lexer* lexer);
    Stmt* stmt_switch(Lexer* lexer);
        SwitchCase* switch_case(Lexer* lexer);
        SVec<SwitchCasePattern*> switch_case_patterns(Lexer* lexer);

    Stmt* stmt(Lexer* lexer);

    SVec<Decl*> parser_loop(Lexer* lexer, TokenKind delim = TK_eof);
    Decl* decl(Lexer* lexer);
    
        Decl* decl_impl(Lexer* lexer);
            SVec<Decl*> parse_impl_body(Lexer* lexer);
        Decl* decl_use(Lexer* lexer);
            SVec<const char*> use_module_names(Lexer* lexer);
            SVec<const char*> use_names(Lexer* lexer);
        Decl* decl_type(Lexer* lexer);
        Decl* decl_base(Lexer* lexer, const char* name, SVec<Note*> notes);
            Decl* decl_constexpr(Lexer* lexer, const char* name);
            Decl* decl_proc(Lexer* lexer, const char* name);
                SVec<ProcParam*> proc_params(Lexer* lexer);
                ProcParam* proc_param(Lexer* lexer);
            Decl* decl_struct(Lexer* lexer, const char* name);            
                Decl* decl_enum(Lexer* lexer, const char* name);
                Decl* decl_aggregate(Lexer* lexer, const char* name, aggregateKind kind);
                Decl* decl_union(Lexer* lexer, const char* name);
                    AggregateItem* aggregate_item(Lexer* lexer);
    
};
#endif /*PARSER_HPP*/