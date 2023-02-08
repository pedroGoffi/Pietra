#ifndef PIETRA_PREP_CPP
#define PIETRA_PREP_CPP

#include "../include/preprocess.hpp"
#include "../include/ast.hpp"
#include "ast.cpp"

using namespace Pietra;
using namespace Pietra::Ast;
using namespace Pietra::Preprocess;


Expr* PreprocessExpr::binary_add(Expr* lhs, Expr* rhs){
    lhs = expr(lhs);
    rhs = expr(rhs);
    if(lhs->kind == rhs->kind){
        
        switch(lhs->kind){
            case Ast::EXPR_INT: {
                lhs->int_lit += rhs->int_lit;
                return lhs;
            }
            case Ast::EXPR_FLOAT:   {
                lhs->float_lit += rhs->float_lit;
                
                return lhs;
            }
            case Ast::EXPR_STRING:                
                lhs->string_lit = Core::cstr((std::string(lhs->string_lit) + std::string(rhs->string_lit)).data());
                return lhs;            
        }
    }
    return nullptr;    
}
Expr* PreprocessExpr::binary_sub(Expr* lhs, Expr* rhs){
    lhs = expr(lhs);
    rhs = expr(rhs);
    if(lhs->kind == rhs->kind){
        
        switch(lhs->kind){
            case Ast::EXPR_INT: {
                lhs->int_lit -= rhs->int_lit;                
                return lhs;
            }
            case Ast::EXPR_FLOAT:   {
                lhs->float_lit -= rhs->float_lit;
                
                return lhs;
            }            
        }
    }    
    return nullptr; 
}
Expr* PreprocessExpr::binary_mult(Expr* lhs, Expr* rhs){
    lhs = expr(lhs);
    rhs = expr(rhs);
    if(lhs->kind == rhs->kind){
        
        switch(lhs->kind){
            case Ast::EXPR_INT: {
                lhs->int_lit *= rhs->int_lit;                
                return lhs;
            }
            case Ast::EXPR_FLOAT:   {
                lhs->float_lit *= rhs->float_lit;
                
                return lhs;
            }            
        }
    }    
    return nullptr; 
}        

Expr* PreprocessExpr::binary(Lexer::tokenKind kind, Expr *lhs, Expr *rhs){
    switch(kind){
        case Lexer::TK_ADD:     return binary_add(lhs, rhs);
        case Lexer::TK_SUB:     return binary_sub(lhs, rhs);
        case Lexer::TK_MULT:    return binary_mult(lhs, rhs);        
    }
    return nullptr;
}
Expr* PreprocessExpr::init_var(Expr* e){
    if(e->init_var.rhs){        
        *e->init_var.rhs = *(PreprocessExpr::expr(e->init_var.rhs)?: e);
    }
    return e;
}
Expr* PreprocessExpr::expr(Expr* e){
    assert(e and "ERR: prep_expr with nullptr expr");

    switch(e->kind){
        
        case ExprKind::EXPR_INT:        return e;
        case ExprKind::EXPR_FLOAT:      return e;
        case ExprKind::EXPR_STRING:     return e;        

        case ExprKind::EXPR_BINARY:            
            return binary(
                e->binary.binary_kind, 
                e->binary.left,
                e->binary.right)    
            ?: e;

        case ExprKind::EXPR_INIT_VAR:   return init_var(e);        
        default: return e;

    }
}
#endif /*PIETRA_PREP_CPP*/