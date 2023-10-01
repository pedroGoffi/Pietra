#ifndef PIETRA_PREP_CPP
#define PIETRA_PREP_CPP

#include "../include/preprocess.hpp"


using namespace Pietra;

Expr* Preprocess::prep_expr(Expr* e){
    assert(e and "ERR: prep_expr with nullptr expr");

    switch(e->kind){

        case ExprKind::EXPR_INT:
        case ExprKind::EXPR_FLOAT:
        case ExprKind::EXPR_STR:
        {
            return e;
        }        

        default:
            printf("[WARN]: could not preprocess expr.\n");
            return e;

    }
}
#endif /*PIETRA_PREP_CPP*/