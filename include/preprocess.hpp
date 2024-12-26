#ifndef PIETRA_PREP
#define PIETRA_PREP
#include "ast.hpp"
#include "bridge.hpp"
#include "lexer.hpp"
#include "smallVec.hpp"
#include "resolve.hpp"

using namespace Pietra;
using namespace Pietra::Ast;
    
namespace Pietra::Preprocess {

    struct PreprocessContext{
        enum {
            ERR,
            OK
        } status;
        SVec<const char*>       strings;                
        SVec<CBridge::CVar*>    gvars;
        

        CBridge::CVar* find_gvar(const char* name){
            for(CBridge::CVar*& var: this->gvars){
                if(var->name == name){
                    return var;
                }
            }

            return nullptr;
        }
        CBridge::CVar* find_gvar(Expr* e){
            if(e->kind == Ast::EXPR_NAME){
                return this->find_gvar(e->name);
            }

            return nullptr;
        }

        void install_gvar(CBridge::CVar* var){
            assert(!this->find_gvar(var->name));

            this->gvars.push(var);
        }
    };

    // Preprocessing expressions
    namespace PreprocessExpr {
        Expr* binary(TokenKind kind, Expr* lhs, Expr* rhs);
        Expr* expr(Ast::Expr* e);

        Expr* binary_add(Expr* lhs, Expr* rhs);
        Expr* binary_sub(Expr* lhs, Expr* rhs);
        Expr* binary_mult(Expr* lhs, Expr* rhs);        

        Expr* init_var(Expr* e);
    };

    
    Resolver::Operand eval_expr(Expr* e, PreprocessContext& ctx);
    void eval_block(SVec<Stmt*> block);
    
};
#endif /*PIETRA_PREP*/