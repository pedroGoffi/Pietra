#ifndef PRETTY_PRINT
#define PRETTY_PRINT
#include "../include/ast.hpp"
#include <bits/types/FILE.h>

#define pp_undefined_kind(_kind) \
    printf("[ERR]: at pPrint::%s, is undefined for kind (%zu)\n", __FUNCTION__, (size_t)(_kind)); \
    exit(1);

#define pprint(...) fprintf(pp_file, __VA_ARGS__)
#define CASE(_kind, ...) case _kind: {__VA_ARGS__} break;
namespace Pietra::pPrint{    
    using namespace Pietra;
    using namespace Pietra::Ast;

    void typespec(TypeSpec* ts);
    void expr(Expr* e);

    int     ident   = 0;
    FILE*   pp_file = stdout;
    
    void ppident(){
        for(int i = 0; i < ident; i ++){
            pprint(" ");
        }

    }
    void typespec(TypeSpec* ts){
        if(!ts) return;
        switch(ts->kind){
            CASE(TYPESPEC_NAME,
                pprint("%s", ts->name);
            )
            CASE(TYPESPEC_ARRAY,
                pprint("(Array of ");
                typespec(ts->base);                
                if(ts->array.size){
                    pprint(" size = ");
                    expr(ts->array.size);
                }
                pprint(")");
            )
            CASE(TYPESPEC_CONST, 
                pprint("(Const ");
                assert(ts->base);
                typespec(ts->base);
                pprint(")");
            )
            CASE(TYPESPEC_POINTER,
                pprint("(Ptr-to ");
                assert(ts->base);
                typespec(ts->base);
                pprint(")");
            )
            
            CASE(TYPESPEC_TEMPLATE,
                pprint("(");
                assert(ts->base);
                assert(ts->templated);
                typespec(ts->base);
                pprint("<");
                typespec(ts->templated);
                pprint(">)");
            )
            default:
                pp_undefined_kind(ts->kind);
        }
    }
    void expr(Expr* e){
        if(!e) return;

      
        switch(e->kind){
            CASE(EXPR_INT, 
                pprint("%li", e->int_lit);
            )
            CASE(EXPR_FLOAT,                
                pprint("%lf", e->float_lit);                
            )
            CASE(EXPR_NAME,
                pprint("%s", e->name);                
            )
            CASE(EXPR_FIELD,                
                pprint("(");
                expr(e->field_acc.parent);
                pprint(".");
                expr(e->field_acc.children);
                pprint(")");
                
            )
            CASE(EXPR_CALL,
                pprint("(call (");
                expr(e->call.base);
                pprint(")(");
                for(int i = 0; i < e->call.args.len(); i++){
                    Expr* ex = e->call.args.data[i];                    
                    expr(ex);
                    if (i + 1 < e->call.args.len()){
                        pprint(", ");
                    }
                }
                pprint("))");
            )
            CASE(EXPR_CAST,
                pprint("(cast (");
                expr(e->cast.expr);
                pprint(") -> (");
                typespec(e->cast.typespec);
                pprint("))");
            )
            CASE(EXPR_ARRAY_INDEX,
                pprint("(");
                pprint(("("));
                expr(e->array.base);                
                pprint(")[");
                expr(e->array.index);
                pprint("])");
            )
            CASE(EXPR_STRING,
                pprint("\"%s\"", e->string_lit);
            )
            CASE(EXPR_INIT_VAR,
                pprint("(Declare %s as ", e->init_var.name);                
                ident++;
                if(e->init_var.type){
                    typespec(e->init_var.type);
                } else {
                    pprint("<auto>");
                }
                pprint(" = ");
                expr(e->init_var.rhs);
                ident--;            
            )
            CASE(EXPR_BINARY,
                ident++;
                pprint("(%s ", tokenKind_repr(e->binary.binary_kind));
                expr(e->binary.left);            
                pprint(" ");
                expr(e->binary.right);                
                pprint(")");
                ident--;
            )
            
            CASE(EXPR_UNARY,
                ident++;
                pprint("(%s ", Lexer::tokenKind_repr(e->unary.unary_kind));
                expr(e->unary.expr);
                pprint(")");
                ident--;
            )
            case EXPR_NONE:
            default:
                pp_undefined_kind(e->kind);
        }
    }
    void stmt(Stmt* st){
        assert(st);
        ppident();

        switch(st->kind){
            CASE(STMT_EXPR,
                expr(st->expr);
                pprint("\n");
            )
            CASE(STMT_RETURN, 
                pprint("(return ");
                expr(st->expr);
                pprint(")\n");
            )
            CASE(STMT_SWITCH,
                pprint("(switch (");
                expr(st->stmt_switch.cond);
                pprint("){\n");
                ident++;
                for(SwitchCase* sc: st->stmt_switch.cases){
                    pprint("(case ");
                    for(SwitchCasePattern* scp: sc->patterns){
                        pprint("[%i -> %i] ", scp->begin, scp->end > 0? scp->end: scp->begin);
                    }
                    pprint("{\n");
                    for(Stmt* node: sc->block) stmt(node);
                    pprint("}\n");
                    
                }
                if(st->stmt_switch.has_default){
                    pprint("(default {\n");
                    
                    for(Stmt* node: st->stmt_switch.default_case) stmt(node);
                    pprint("}\n");
                }
                ident--;
                ppident();
                pprint("}\n");
            )
            CASE(STMT_IF, {                
                IfClause* ifc           = st->stmt_if.if_clause;
                SVec<IfClause*> elifs   = st->stmt_if.elif_clauses;
                SVec<Stmt*> else_block  = st->stmt_if.else_block;

                
                pprint("(if ");
                expr(ifc->cond);
                pprint("\n");

                ident++;
                for(Stmt* node: ifc->block) stmt(node);
                ident--;

                ppident();
                pprint(")\n");

                if(elifs.len() > 0){
                    for(IfClause* elifc: elifs){
                        pprint("(elif ");
                        expr(elifc->cond);
                        pprint("\n");
                        ident++;
                        for(Stmt* node: elifc->block) stmt(node);
                        ident--;
                        pprint(")\n");
                        
                    }
                }

                if(else_block.len() > 0){
                    pprint("(else\n");
                    ident++;
                    for(Stmt* node: else_block) stmt(node);
                    ident--;
                    pprint(")\n");
                }
                
                
            })
            CASE(STMT_FOR,
                Expr*       init    = st->stmt_for.init;
                Expr*       cond    = st->stmt_for.cond;
                Expr*       inc     = st->stmt_for.inc;
                SVec<Stmt*> block   = st->stmt_for.block;

                pprint("(for [");
                expr(init);
                pprint(", ");
                expr(cond);
                pprint(", ");
                expr(inc);
                pprint("]{\n");
                ident++;
                for(Stmt* node: block) stmt(node);
                ident--;
                pprint("}\n");
            )
            CASE(STMT_WHILE,
                assert(st->stmt_while);
                pprint("(while ");                
                ident++;
                expr(st->stmt_while->cond);
                pprint("\n");

                for(Stmt* node: st->stmt_while->block){
                    stmt(node);
                }
                ident--;
                pprint(")\n");
            )

      
            default: pp_undefined_kind(st->kind);
        }
    }
    void decl(Decl* d){        
        assert(d);
        
        switch(d->kind){
            CASE(DECL_TYPE,
                pprint("(Type %s = ", d->name);
                typespec(d->type_alias.type);
                pprint(")\n");
            )
            CASE(DECL_CONSTEXPR,
                assert(d->expr);
                pprint("(constexpr %s = ", d->name);
                expr(d->expr);
                pprint(")\n");
            )

            CASE(DECL_PROC,
                if(!d->proc.is_complete) return;
                pprint("(proc %s ", d->name);
                
                for(ProcParam* param: d->proc.params){                    
                    pprint("(param %s ", param->name);
                    typespec(param->type);                    
                    if(param->init){
                        pprint(" = ");
                        expr(param->init);
                    }
                    pprint("), ");
                }
                if(d->proc.ret){
                    pprint("(returns ");
                    typespec(d->proc.ret);
                    pprint(") ");
                }
                
                if(d->proc.is_complete){
                    pprint("{\n");
                    ident++;
                    for(Stmt* node: d->proc.block) stmt(node);
                    ident--;
                    pprint("}");
                }
                pprint(")\n");
            )
            CASE(DECL_ENUM,
                pprint("(enum %s\n", d->name);
                for(int i = 0; i < d->enumeration.items.len(); i++){
                    EnumItem& item = *d->enumeration.items.at(i);                    
                    pprint("[%s ", item.name);
                    if(item.init){
                        pprint(" = ");
                        expr(item.init);
                    }
                    pprint("]\n");
                }
                pprint(")\n");
            )
            CASE(DECL_AGGREGATE,
                bool isStruct = d->aggregate.aggkind == AGG_STRUCT;
                if(!isStruct) assert(d->aggregate.aggkind == AGG_UNION);

                pprint("(Aggregate (%s) {\n", isStruct ? "struct" : "union");
                for(AggregateItem* item: d->aggregate.items){
                    assert(item->kind == AGGREGATEITEM_FIELD);

                    pprint("(fields [");
                    for(const char* name: item->field.names) pprint("%s, ", name);
                    pprint("] as ");
                    if(item->field.type){
                        typespec(item->field.type);
                    } else pprint("<auto> "); 

                    if(item->field.init){
                        pprint(" = ");
                        expr(item->field.init);
                    }

                    pprint(")\n");                    
                }
                pprint("}\n");
            )
            CASE(DECL_USE,                
                pprint("(use {");
                if(d->use.use_all){
                    pprint("...");
                }
                
                pprint("} ");                
                
                if(d->use.rename){
                    pprint(" as %s", d->use.rename);
                }

                pprint(")\n");
            )
            CASE(DECL_VAR,
                pprint("(declare global %s as ", d->name);
                if(d->var.type){
                    typespec(d->var.type);                    
                } else {
                    pprint("<auto>");
                }

                if(d->var.init){
                    pprint(" = ");
                    expr(d->var.init);
                }
                pprint(")\n");

            )
            CASE(DECL_IMPL, 
                pprint("(impl %s {\n", d->impl.target);
                for(Decl* node: d->impl.body){
                    decl(node);                    
                }
                pprint("}\n");
            )
            default: pp_undefined_kind(d->kind);
        }
    }
};
#undef CASE
#undef pp_undefined_kind
#endif /*PRETTY_PRINT*/