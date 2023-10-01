#ifndef CGEN_CPP
#define CGEN_CPP
#include "../include/cGen.hpp"
#include <cstring>
using namespace Pietra::cGen;

#define DEBUG               true
#define OUT_FD_COMPILE      cGen::out_file
#define OUT_FD_DEBUG        stdout
#define OUT_FD              (DEBUG)? OUT_FD_DEBUG: OUT_FD_COMPILE
#define printf(...)         fprintf(OUT_FD, __VA_ARGS__)

namespace Pietra::cGen {
    FILE* out_file;
}

void cGen::gen_init_var(const char* name, TypeSpec* type, Expr* expr){
    assert(type);
    gen_type(type);
    printf(" %s", name);
    
    if(expr){
        printf(" = ");
        gen_expr(expr);
    }
    printf(";");
}

void cGen::gen_type(TypeSpec* type){
    if(!type) {
        printf("auto"); 
        return;
    }

    switch(type->kind){
        case Ast::TYPESPEC_NAME:
            printf("%s", type->name);
            break;
        case Ast::TYPESPEC_CONST:
            printf("const ");
            gen_type(type->base);
            break;
        case Ast::TYPESPEC_POINTER:
            assert(type->base);
            printf("");
            gen_type(type->base);
            printf("*");
            break;

        default: assert(0);
    }
}
void cGen::gen_expr(Expr* e){
    assert(e);

    switch(e->kind){
        case EXPR_INT:      
            printf("(%li)", e->int_lit); 
            break;
        case EXPR_FLOAT:    
            printf("(%lf)", e->float_lit); 
            break;
        case EXPR_STRING:
        {
            printf("\"%s\"", e->string_lit);
        } break;
        case EXPR_BINARY:
        {
            const char* repr = tokenKind_repr(e->binary.binary_kind);
            printf("(");
            gen_expr(e->binary.left);
            printf(" %s ", repr);
            gen_expr(e->binary.right);
            printf(")");
            
            break;
        } 
        case EXPR_NAME: 
            printf("%s", e->name); 
            break;
        case EXPR_CALL:
        {   
            printf("(");
            gen_expr(e->call.base);
            printf(")(");
            for(int i = 0; i < e->call.args.len(); i++){
                Expr* arg = e->call.args.data[i];
                assert(arg);

                gen_expr(arg);
                printf(i + 1 < e->call.args.len()? ", ": "");                
            }
            printf(")");
            break;            
        }  
        case EXPR_FIELD:
        {
            printf("(");
            gen_expr(e->field_acc.parent);            
            printf(".");
            gen_expr(e->field_acc.children);
            printf(")");
        } break;
        case EXPR_INIT_VAR:
            gen_init_var(
                e->init_var.name,
                e->init_var.type,
                e->init_var.rhs
            );
            break;
        

        default: assert(0);
    }
}
void cGen::gen_stmt(Stmt* stmt){
    assert(stmt);

    switch(stmt->kind){
        case STMT_EXPR: 
            gen_expr(stmt->expr);
            printf(";\n");
            break;
        case STMT_RETURN: 
            printf("return ");
            gen_expr(stmt->expr);
            printf(";\n");

        case STMT_FOR:
        {
            Expr* init  = stmt->stmt_for.init;
            Expr* cond  = stmt->stmt_for.cond;
            Expr* inc   = stmt->stmt_for.inc;

            printf("for ");
            if(!init and !cond and !inc){
                printf("(;;)");
            }
            else {
                assert(init->kind == EXPR_INIT_VAR);

                printf("(");
                gen_expr(init);
                // NOTE: ";" printed inside gen_expr for init var
                gen_expr(cond);
                printf(";");
                gen_expr(inc);
                printf(")");

            }


            printf("{\n");
            gen_stmt_block(stmt->stmt_for.block);
            printf("}");
            break;
        }
        case STMT_SWITCH:
            printf("switch(");
            gen_expr(stmt->stmt_switch.cond);
            printf("){\n");
            for(SwitchCase* c: stmt->stmt_switch.cases){
                for(SwitchCasePattern* pattern: c->patterns){
                    if(pattern->name){
                        printf("case %s:", pattern->name);
                    }
                    
                    for(char p = pattern->begin; p <= pattern->end; p++){
                        printf("case %i: ", p);
                    }                    
                }
                printf("{\n");
                gen_stmt_block(c->block);
                printf("} break;\n");
            }

            if(stmt->stmt_switch.has_default){
                printf("default: {\n");
                gen_stmt_block(stmt->stmt_switch.default_case);
                printf("} break;\n");
            }
            printf("}\n");
            break;
        case STMT_IF:
        {
            printf("if(");
            gen_expr(stmt->stmt_if.if_clause->cond);
            printf(")");
            gen_stmt_block(stmt->stmt_if.if_clause->block);

            for(IfClause* elif: stmt->stmt_if.elif_clauses){
                printf("else if(");
                gen_expr(elif->cond);
                printf(")");
                gen_stmt_block(elif->block);
            }
            if(stmt->stmt_if.else_block.len() > 0){
                printf("else ");
                gen_stmt_block(stmt->stmt_if.else_block);
            }
        } break;

        default: assert(0);
    }
}
void cGen::gen_stmt_block(SVec<Stmt*> block){
    printf("{\n");
    for(Stmt* node: block) gen_stmt(node);
    printf("}\n");
}
void cGen::gen_decl(Decl* decl){
    assert(decl);
    printf("/////////////////////////////////////////\n");
    switch(decl->kind){
        case DECL_VAR:
            gen_init_var(decl->name, decl->var.type, decl->var.init);
            printf("\n");
            break;
        case DECL_CONSTEXPR:
            printf("#define %s ", decl->name);
            gen_expr(decl->expr);
            printf("\n");
            break;
        case DECL_TYPE:        
            printf("typedef ");
            gen_type(decl->type_alias.type);
            printf(" %s;\n", decl->name);
            break;
        case DECL_AGGREGATE:
        {
            bool isStruct = decl->aggregate.aggkind == AGG_STRUCT;
            if(!isStruct) assert(decl->aggregate.aggkind == AGG_UNION);
            const char* aggName = isStruct? "struct": "union";
            printf("typedef %s %s {\n", aggName, decl->name);
            for(AggregateItem* item: decl->aggregate.items){
                switch(item->kind){
                    case AGGREGATEITEM_FIELD:
                        for(const char* name: item->field.names){
                            gen_init_var(name, item->field.type, item->field.init);
                            printf("\n");
                        }
                        break;
                    default: assert(0);
                }
            }
            printf("} %s;\n", decl->name);

        } break;
        case DECL_ENUM:
            printf("enum %s{\n", decl->name);
            for(EnumItem* item: decl->enumeration.items){
                printf("%s", item->name);
                if(item->init){
                    printf(" = ");
                    gen_expr(item->init);                    
                }
                printf(",\n");
            }
            printf("}\n");
            break;
        case DECL_PROC:            
        
            gen_type(decl->proc.ret);
            
            printf(" %s", decl->name);
            {
                printf("(");
                for(int i = 0; i < decl->proc.params.len(); i++){
                    ProcParam* param = decl->proc.params.data[i];
                    assert(param);

                    gen_type(param->type);
                    printf(" %s", param->name);
                    if(param->init){
                        printf(" = ");
                        gen_expr(param->init);
                    }

                    printf(i + 1 < decl->proc.params.len()? ", ": "");
                }
                printf(")");
            }
            gen_stmt_block(decl->proc.block);
            break;
        default: assert(0);
    }
}

void cGen::gen_all(SVec<Sym*> ast, const char* out_fp){
    #if not DEBUG
    cGen::out_file = fopen(out_fp, "w");
    assert(cGen::out_file);
    #endif


    const char* stdlibs[] = {
        "<iostream>",        
    };

    for(const char* lib: stdlibs) printf("#include %s\n", lib);
    for(Sym* sym: ast){
        gen_decl(sym->decl);
    }        
}
#undef printf
#endif /*CGEN_CPP*/