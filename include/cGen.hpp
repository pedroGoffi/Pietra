#ifndef CGEN_HPP
#define CGEN_HPP
#include "ast.hpp"
#include "resolve.hpp"
#include "smallVec.hpp"
#include "interns.hpp"
using namespace Pietra::Core;
using namespace Pietra::Resolver;

namespace Pietra::cGen{
    void gen_init_var(const char* name, TypeSpec* type, Expr* expr);
    void gen_type(TypeSpec* type);
    void gen_expr(Expr* expr);
    void gen_stmt(Stmt* stmt);
    void gen_stmt_block(SVec<Stmt*> block);
    void gen_decl(Decl* decl);
    void gen_all(SVec<Decl*> ast, const char* out_fp);
}

#endif /*CGEN_HPP*/