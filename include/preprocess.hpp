#ifndef PIETRA_PREP
#define PIETRA_PREP
#include "ast.hpp"

namespace Pietra::Preprocess {
    using namespace Pietra;

    Ast::Expr* prep_expr(Ast::Expr* e);
};
#endif /*PIETRA_PREP*/