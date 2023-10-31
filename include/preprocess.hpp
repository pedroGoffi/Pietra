#ifndef PIETRA_PREP
#define PIETRA_PREP
#include "ast.hpp"
#include "lexer.hpp"

using namespace Pietra;
using namespace Pietra::Ast;
    
namespace Pietra::Preprocess {
    

    // Preprocessing expressions
    namespace PreprocessExpr {
        Expr* binary(Lexer::tokenKind kind, Expr* lhs, Expr* rhs);
        Expr* expr(Ast::Expr* e);

        Expr* binary_add(Expr* lhs, Expr* rhs);
        Expr* binary_sub(Expr* lhs, Expr* rhs);
        Expr* binary_mult(Expr* lhs, Expr* rhs);        

        Expr* init_var(Expr* e);
    };

    
};
#endif /*PIETRA_PREP*/