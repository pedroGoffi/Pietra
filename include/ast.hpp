#ifndef AST_HPP
#define AST_HPP
#include <cstdint>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "smallVec.hpp"
namespace Pietra::Ast { struct Type; } // extern 

namespace Pietra::Ast {
    using namespace Pietra;        
    using namespace Pietra::Core; 

    struct Expr;
    struct Stmt;
    struct TypeSpec;

    struct Note{
        const char* name;
        SVec<Expr*> args;
    };
    enum ExprKind{
        EXPR_NONE,                      //  Undefined    
        EXPR_INT,                       //  integers           
        EXPR_FLOAT,                     //  floating point     
        EXPR_STRING,                    //  c based string literal      
        EXPR_NAME,                      //  name identifier    
        EXPR_INIT_VAR,                  //  name :type = expr
        EXPR_COMPOUND,                  //  (:type)* {expr}
        EXPR_FIELD,                     //  expr ". :: " expr
        EXPR_COMPOUND_LIST_INDEXING,    //  [expr] = expr
        EXPR_COMPOUND_NAME,             //  :name = expr
        EXPR_NEW,                       //  new int
        EXPR_UNARY,                     //  -expr
        EXPR_BINARY,                    //  expr BIN_OP expr        
        EXPR_CMP,                       //  expr == expr
        EXPR_LOGIC,                     //  expr and expr
        EXPR_CAST,                      //  expr as type | (:type) expr
        EXPR_TERNARY,                    //  expr if expr else expr
        EXPR_ARRAY_INDEX,
        EXPR_CALL,        
    };
    
    struct Expr{
        ExprKind kind;
        union {
            uint64_t    int_lit;
            double      float_lit;
            const char* string_lit;
            const char* name;
            struct {
                Expr*        base;
                SVec<Expr*>  args;
            } call;
            struct {
                Expr* base;
                Expr* index;
            } array;
            struct {
                const char* name;
                TypeSpec*   type;
                Expr*       rhs;
            } init_var;
            struct {
                Expr* parent;
                Expr* children;
            } field_acc;
            struct {
                TypeSpec     *typespec;
                SVec<Expr*>  expr_list;
            } compound;
            struct {
                Expr* base_expr;
                Expr* rhs;
            } list_indexing;
            struct {
                const char* name;
                Expr*       rhs;
            } name_indexing;
            struct {
                Lexer::tokenKind unary_kind;
                Expr*            expr;
            } unary;
            struct {
                Lexer::tokenKind    binary_kind;
                Expr*               left;
                Expr*               right;
            } binary;
            struct {
                Expr* cond;                
                Expr* if_case;
                Expr* else_case;
            } ternary;
            struct {
                TypeSpec* typespec;
                Expr* expr;
            } cast;
        };
    };
    enum StmtKind{
        STMT_NONE,
        STMT_EXPR, //
        STMT_IF,
        STMT_SWITCH, //
        STMT_FOR, 
        STMT_WHILE,   //
        STMT_RETURN
    };
    
    struct SwitchCasePattern{        
        char begin;
        char end;                    
        const char* name;
        const char* string;        
    };
    struct SwitchCase{
        SVec<SwitchCasePattern*>    patterns;
        SVec<Stmt*>                 block;        
    };
    struct StmtWhile{
        Expr*       cond;
        SVec<Stmt*> block;
        bool        is_doWhile;
    };
    
    struct IfClause {
        Expr*       cond;
        SVec<Stmt*> block;
    };    
    struct Stmt{
        StmtKind kind;
        union {
            StmtWhile*  stmt_while;
            struct {
                IfClause*       if_clause;
                SVec<IfClause*> elif_clauses;        
                SVec<Stmt*>     else_block;
            } stmt_if;
            struct {
                Expr*       init;
                Expr*       cond;
                Expr*       inc;
                SVec<Stmt*> block;
            } stmt_for;
            struct {
                Expr*               cond;
                SVec<SwitchCase*>   cases;
                bool                has_default;
                SVec<Stmt*>         default_case;
            } stmt_switch;
            Expr* expr;
        };
    };


    enum TypeSpecKind{
        TYPESPEC_NONE,
        TYPESPEC_NAME,
        TYPESPEC_POINTER,
        TYPESPEC_ARRAY,
        TYPESPEC_PROC,
        TYPESPEC_CONST,
        TYPESPEC_TEMPLATE
    };
    
    struct TypeSpec{
        Lexer::Token        token;
        TypeSpecKind        kind;
        bool                mutablity;
        TypeSpec*           base;    
        Type*               resolvedTy; 
        union{            
            const char* name;
            struct {                
                Expr*       size;
            } array;
            struct {                
                SVec<TypeSpec*> params;
                TypeSpec*       ret;
                bool            has_varags;
            } proc;
            TypeSpec* templated;
        };
    };

   
    enum AggregateItemKind{
        AGGREGATEITEM_NONE,
        AGGREGATEITEM_FIELD,
        AGGREGATEITEM_PROC,
        AGGREGATEITEM_SUBAGGREGATE,                
    };
    struct ProcParam{
        const char* name;
        bool        isVararg;
        TypeSpec*   type;
        Expr*       init;
    };
    struct AggregateItem {        
        AggregateItemKind kind;
        union {            
            struct {                
                SVec<const char*>   names;
                TypeSpec*           type;
                Expr*               init;
            } field;
        };
    };
    struct EnumItem{
        const char* name;
        Expr*       init;
    };
    enum DeclKind {
        DECL_NONE,
        DECL_VAR,
        DECL_PROC,
        DECL_AGGREGATE,        
        DECL_ENUM,
        DECL_USE,        
        DECL_TYPE,
        DECL_CONSTEXPR,
        DECL_IMPL
    };
    
    enum aggregateKind { AGG_NONE,  AGG_STRUCT, AGG_UNION };
    struct Decl{
        DeclKind    kind;
        const char* name;
        SVec<Note*> notes;
        union {
            struct {
                aggregateKind           aggkind;
                SVec<AggregateItem*>    items;
            } aggregate;

            struct {
                TypeSpec*   type;
                Expr*       init;
            } var;

            struct {                            
                bool                is_complete; 
                bool                is_vararg;
                bool                is_internal;
                SVec<ProcParam*>    params;
                TypeSpec*           ret;
                SVec<Stmt*>         block;
            } proc;
            struct {                                
                SVec<EnumItem*> items;
            } enumeration;
            struct {
                TypeSpec* type;
            } type_alias;
            struct {
                SVec<Decl*>         module;
                bool                use_all;                
                const char*         rename;
            } use;

            struct {
                const char* target; //  the target is the string ptr to the array|type item
                //                      for example: impl i64 { ... }
                SVec<Decl*> body;
                // The body of the implementation
            } impl;
            Expr* expr;
        };
        

    };
}



#endif /*AST_HPP*/