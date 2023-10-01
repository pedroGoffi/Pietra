/*
    The resolver will
    [1]: order the AST
    [2]: TODO: type checking
    [3]: TODO: const evaluation    
*/
#ifndef RESOLVE_CPP
#define RESOLVE_CPP
#include "../include/resolve.hpp"
#include "type.cpp"
#include "ast.cpp"
#include "interns.cpp"
#include <cstring>
#include "pprint.cpp"
#include "package.cpp"

using namespace Pietra;
using namespace Pietra::Ast;
using namespace Pietra::Resolver;
#define TODO(...) printf("TODO: IN: %s: ", __FUNCTION__); printf(__VA_ARGS__);
SVec<Sym*>  declared_syms;
SVec<Sym*>  resolved_syms;
SVec<Sym*>  local_syms;
SVec<Decl*> ordered_decls;
const char* cpName;

std::map<const char*, SVec<TypeField*>> procs_local_vars;
Sym* Resolver::sym_get(const char* name){
    for(Sym* sym: declared_syms){
        if(sym->name == name){
            return sym;
        }
    }        
    for(Sym* sym: local_syms){
        if(sym->name == name){
            return sym;
        }    
    }    
    
    return nullptr;    
}
void Resolver::sym_push(Sym* sym){
    declared_syms.push(sym);
}
void Resolver::resolved_sym_push(Sym* sym){
    resolved_syms.push(sym);
}
Sym* init_sym(SymKind kind){
    Sym* s = Core::arena_alloc<Sym>();
    assert(s);
    s->kind = kind;
    return s;
}
Sym* sym_var(const char* name, Type* type){
    Sym* s = init_sym(SYM_VAR);
    s->name = name;
    s->type = type;
    return s;
}
Sym* Resolver::new_sym_decl(SymKind kind, const char* name, Decl*& decl){
    Sym* s = init_sym(kind);
    assert(s);
    assert(decl);
    s->name  = name;
    s->decl = arena_alloc<Decl>();
    *s->decl = *decl;    
    s->state = SYM_UNRESOLVED;
    return s;
}

Sym* Resolver::sym_from_decl(Decl* decl){
    if(sym_get(decl->name)){
        printf("[ERROR]: the declaration: %s is already declared.\n", decl->name);
        exit(1);
    }
    assert(decl);
    
    SymKind kind = SYM_NONE;    
    Ast::Type* type = nullptr;
    Sym* sym = new_sym_decl(kind, decl->name, decl);
    switch(decl->kind){
        case DECL_TYPE:
            kind = SYM_TYPE_ALIAS;            
            break;
        case DECL_VAR:
            kind = SYM_VAR;            
            break;
        case DECL_CONSTEXPR:
            kind = SYM_CONSTEXPR;
            type = type_int(64);
            break;
        case DECL_PROC:{            
            kind = SYM_PROC;
            SVec<TypeField*> params;
            Type* ret = resolve_typespec(decl->proc.ret);
            for(ProcParam* param: decl->proc.params){
                Type* param_t = resolve_typespec(param->type);                                
                TypeField* tf = init_typefield(param->name, param_t);
                params.push(tf);                                
                local_syms.push(sym_var(param->name, param_t));
                            
            }

            type = type_proc(params, ret);
        }   break;

        case DECL_AGGREGATE:
            kind = SYM_AGGREGATE;
            SVec<TypeField*> items;

            for(AggregateItem* item: decl->aggregate.items){
                
                switch(item->kind){
                    case Ast::AGGREGATEITEM_FIELD:
                        for(auto name: item->field.names){
                            Type* type = resolve_typespec(item->field.type);
                            TypeField* item = Utils::init_typefield(name, type);
                            assert(item);
                            items.push(item);
                        }
                        break;
                    default: assert(0 && "unreachable");
                }
            }

            type = type_aggregate(items, decl->aggregate.aggkind == Ast::AGG_STRUCT);
            break;
    }
    if(!type){
        type = type_unresolved();
    }
    
    sym->kind = kind; 
    sym->type = type;    
    return sym;
        
}
Sym* sym_from_type(Ast::Type* type){
    Sym* s      = init_sym(SYM_TYPE);
    s->type     = type;
    s->state    = SYM_RESOLVED;
    return s;
}
void declare_type(const char* name, Ast::Type* type){
    assert(!sym_get(name));
    Sym* s = sym_from_type(type);
    s->kind = SYM_TYPE;
    s->type = type;
    s->name = Core::cstr(name);
    declared_syms.push(s);
    resolved_syms.push(s);
}

void Resolver::init_builtin_syms(){        
    declare_type("null", type_void());
    declare_type("i8", type_int(8));
    declare_type("i16", type_int(16));
    declare_type("i32", type_int(32));
    declare_type("i64", type_int(64));

    declare_type("f32", type_float(32));
    declare_type("f64", type_float(64));    

    declare_type("cstr", type_ptr(type_int(8)));
    {
        ProcParam* tmp = arena_alloc<ProcParam>();    
        SVec<ProcParam*> params;
        params.push(tmp);
        declare_decl(Utils::decl_proc("__print", params, Utils::typespec_name("null"), {}, false));
        declare_decl(Utils::decl_proc("__syscall", params, Utils::typespec_name("cstr"), {}, false));
        
    }
}

void Resolver::declare_decl(Decl* decl){            
    Sym* sym = sym_from_decl(decl);
    assert(sym);
    printf("\t[NOTE]: DECLARING: %s\n", sym->name);
    declared_syms.push(sym);
}

#define CASE(_KIND, ...) case _KIND: {__VA_ARGS__}; break;
void Resolver::resolve_init_var(const char* name, TypeSpec* typespec, Expr*& init){
    Sym* sym = sym_get(name);        
    Type* type = resolve_typespec(typespec);
    if(!init) {
        // Zero initialize the data
        init = Utils::expr_int(0);
    }
    resolve_expr(init);
}
Type* Resolver::type_from_typespec(TypeSpec* typespec){
    switch(typespec->kind){    
        
        case TYPESPEC_NAME: return resolve_typespec(typespec);
        case TYPESPEC_POINTER:  
            return type_ptr(resolve_typespec(typespec->base));
        
        case TYPESPEC_ARRAY:
        case TYPESPEC_PROC:
        case TYPESPEC_CONST:
        case TYPESPEC_NONE:
        case TYPESPEC_TEMPLATE:
        default: assert(0 && "could not get the type from this typespec");
    }
}
Type* Resolver::resolve_typespec(TypeSpec*& typespec){
    if(!typespec){
        return Ast::type_void();
    }

    switch(typespec->kind){
        CASE(TYPESPEC_NAME,                
            Sym* sym = sym_get(typespec->name);                        
            if(!sym){
                printf("[WARN]: the type: %s is not declared.\n", typespec->name);
                exit(1);
            }
            Sym* ty = resolve_name(sym->name);            
            assert(ty->type);
            typespec->resolved_type = ty->type;            
            return ty->type;
        )       
        CASE(TYPESPEC_ARRAY)
        CASE(TYPESPEC_POINTER,
            typespec->resolved_type = type_from_typespec(typespec);

        )
        default:    assert(0);
    }
}
////////////////////////////////////////////////////////////////////
//////////////////////  RESOLVING EXPRS  ///////////////////////////
////////////////////////////////////////////////////////////////////
Type* Resolver::resolve_expr(Expr* expr){    

    assert(expr);

    switch(expr->kind){
        CASE(EXPR_INT,      return type_int(64);)
        CASE(EXPR_STRING,   return type_string();)
        CASE(EXPR_FLOAT,    return type_float(64);)
        CASE(EXPR_NAME,             
            Sym* sym = resolve_name(expr->name);
            assert(sym->type);
            return sym->type;
        )            
        CASE(EXPR_UNARY, 
            // TODO: typechecking
            resolve_expr(expr->unary.expr); 
        )
        CASE(EXPR_BINARY,
            resolve_expr(expr->binary.left);
            resolve_expr(expr->binary.right);
        )
        CASE(EXPR_CALL,
            resolve_expr(expr->call.base);
        )
        CASE(EXPR_INIT_VAR,
            const char* name    = expr->init_var.name;
            Type* type          = resolve_typespec(expr->init_var.type);
            if(expr->init_var.rhs) resolve_expr(expr->init_var.rhs);            
            procs_local_vars[cpName].push(init_typefield(name, type));                        
            local_syms.push(sym_var(name, type));            
        )
        CASE(EXPR_FIELD)
        default: 
            printf("--------------------\n");
            pPrint::expr(expr);
            printf("--------------------\n");
            assert(0);
    }
}
////////////////////////////////////////////////////////////////////
//////////////////////  RESOLVING STMTS  ///////////////////////////
////////////////////////////////////////////////////////////////////
void Resolver::resolve_stmt(Stmt* st){
    assert(st);
    switch(st->kind){
        CASE(STMT_EXPR, resolve_expr(st->expr);)
        CASE(STMT_IF, fprintf(stderr, "[WARN- TODO]: resolve-if\n");)
        CASE(STMT_WHILE, fprintf(stderr, "[WARN- TODO]: resolve-while\n");)
        CASE(STMT_RETURN, resolve_expr(st->expr);)
        CASE(STMT_FOR,
            if(st->stmt_for.init) resolve_expr(st->stmt_for.init);
            if(st->stmt_for.cond) resolve_expr(st->stmt_for.cond);
            if(st->stmt_for.inc)  resolve_expr(st->stmt_for.inc);
            
        )
        
        default: 
            printf("[WARN]: Undefined resolving for:\n");
            pPrint::stmt(st);
            printf("---------------------------\n");
    }
}

void Resolver::resolve_stmt_block(SVec<Stmt*> block){
    for(Stmt* stmt: block) resolve_stmt(stmt);
}
////////////////////////////////////////////////////////////////////
//////////////////////  RESOLVING DECLS  ///////////////////////////
////////////////////////////////////////////////////////////////////
void Resolver::resolve_decl_constexpr(Decl* decl){
    assert(decl->kind == DECL_CONSTEXPR);
    resolve_expr(decl->expr);
}
void Resolver::resolve_decl_var(Decl* dvar){
    assert(dvar->kind == DECL_VAR);        
    resolve_init_var(dvar->name, dvar->var.type, dvar->var.init);
}

void Resolver::resolve_decl_aggregate(Decl* decl){
    assert(decl->kind == DECL_AGGREGATE);
    aggregateKind kind = decl->aggregate.aggkind;
    
    for(AggregateItem* item: decl->aggregate.items){
        AggregateItemKind item_kind = item->kind;

        switch(item_kind){
            CASE(AGGREGATEITEM_FIELD, 
                if(item->field.init == nullptr){
                    item->field.init = Utils::expr_name("null");
                }
                for(const char* name: item->field.names){                    
                    resolve_init_var(name, item->field.type, item->field.init);                    
                }
            )
            default: assert(0);
        }
    }
        
}
void resolve_decl_use(Decl* decl){
    assert(decl->kind == DECL_USE);
    std::string fp;
    for(const char* p: decl->use.module_names){
        if(!fp.empty()){
            fp += "/";
        }
        fp += p;
        
    }
    fp += ".pi"; // extension
    if(fp.at(0) != '/'){
        std::string tmp = "/home/robot/Desktop/Pietra/";
        tmp += fp;
        fp = tmp;
    }
    

    
    PPackage* package = PPackage::from(fp.c_str());    
    for(Decl* decl: package->ast){        
        Sym* sym = sym_from_decl(decl);
        assert(sym);
        sym->state = SYM_RESOLVED;
        

        declared_syms.push(sym);
        resolved_syms.push(sym);            
    }    
}

void Resolver::resolve_decl_proc(Decl* decl){    
    
    assert(decl->kind == DECL_PROC);
    Sym* sym = sym_get(decl->name);
    
    assert(sym);
    assert(sym->decl == decl);

    if(sym->type->kind == TYPE_UNRESOLVED){
        // Resolve the type
        SVec<TypeField*>    params;        
        for(ProcParam* param: decl->proc.params){
            printf("param->name == %s\n", param->name );
        
            Type* type = resolve_typespec(param->type);
            // Declare the name
            
            TypeField* tf = arena_alloc<TypeField>();
            
            //tf->name = Core::cstr(param->name);
            tf->type = type;
            params.push(tf);
            
        }
        Type* ret = decl->proc.ret
            ? resolve_typespec(decl->proc.ret)
            : type_void();

        *sym->type = *type_proc(params, ret);
        resolve_typespec(sym->decl->proc.ret);
    }
    cpName = decl->name;    
    resolve_stmt_block(decl->proc.block);    
    cpName = nullptr;        
}
void Resolver::resolve_decl(Decl* decl){    
    assert(decl);
    switch(decl->kind){        
        CASE(DECL_VAR,          resolve_decl_var(decl);)
        CASE(DECL_CONSTEXPR,    resolve_decl_constexpr(decl);)
        CASE(DECL_PROC,         resolve_decl_proc(decl);)
        CASE(DECL_AGGREGATE,    resolve_decl_aggregate(decl);)
        CASE(DECL_USE,          resolve_decl_use(decl);)
        case DECL_TYPE: break;                              
        case DECL_ENUM:        
        case DECL_NONE:      
        default: {
            printf("[ERROR]: Can't resolve this ============================================\n");
            pPrint::decl(decl);            
            printf("========================================================================\n");
            exit(1);
        }
    }
    ordered_decls.push(decl);
}
#undef CASE
void Resolver::resolve_sym(Sym* sym){    
    if(sym->state == SYM_RESOLVED) return;
    if(sym->state == SYM_RESOLVING){
        printf("[ERROR]: Cyclic dependence on: %s\n", sym->name);
        exit(1);
    }

    
    if(sym->decl){
        sym->state = SYM_RESOLVING;     
        resolve_decl(sym->decl);
        sym->state = SYM_RESOLVED;        
        resolved_syms.push(sym);        
    } else {
        sym->state = SYM_RESOLVED;
    }              
}
Sym* Resolver::resolve_name(const char* name){
    Sym* sym = sym_get(name);
    if(!sym){
        printf("[ERR]: the name %s is not defined.\n", name);
        exit(1);
    }
    resolve_sym(sym);
    return sym;
}

SVec<Decl*>  Resolver::resolve_ast(SVec<Decl*> ast){
    init_builtin_syms();
    for(Decl* decl: ast){                                
        declare_decl(decl);
    }
    

    for(Sym* sym: declared_syms){                        
        resolve_sym(sym);                
    }
    return ordered_decls;
}


#endif /*RESOLVE_CPP*/