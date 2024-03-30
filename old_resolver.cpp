/*
The resolver will
[1]: order the AST
[2]: TODO: type checking
[3]: TODO: const evaluation    
*/
#ifndef RESOLVE_CPP
#define RESOLVE_CPP
#include "../include/resolve.hpp"
#include "../include/packages.hpp"
#include "../include/bridge.hpp"
#include "ast.cpp"
#include "pprint.cpp"    
#include "bridge.cpp"
#include "preprocess.cpp"
#include <clocale>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

bool impl_ctx = false;
Decl* string_comparator = 0;
Note* find_note(SVec<Note*> notes, const char* name){    
    name = Core::cstr(name);    
    for(Note* note: notes){
        if(note->name == name) return note;
    }

    return nullptr;
}
void show_all_decorators(){
    /*
        NOTE: those macros serve to use in the printf loop
        they check if the item in the iteration is the last one
        if the item is the last one in the list we dont print the ", "
        otherwise we separate the itens with ", "
    */
    #define LIST_SIZEOF(__list)     (sizeof((__list)) / sizeof((__list)[0]))
    #define LIST_LAST_ELEM(__list)  (__list)[LIST_SIZEOF(__list) - 1]
    #define SAD_printf_cond(item, __list) ((item) == LIST_LAST_ELEM(__list))? "": ", " 
    static const char* decorators_name[] = {
        "todo", "inline", "error", "warn", "string_comparator"
    };
    const char* last_decorator = LIST_LAST_ELEM(decorators_name);
        
    printf("Decorators = [");
    for(const char* dc: decorators_name){
        printf("@%s%s", 
            dc, 
            SAD_printf_cond(dc, decorators_name));
    }
    printf("]\n");
    #undef LIST_SIZEOF
    #undef LIST_LAST_ELEM
    #undef SAD_printf_cond
}
namespace Pietra::Resolver{
    SVec<Sym*>          Syms;
    SVec<Sym*>          localSyms;
    SVec<Decl*>         final_ast;
    SVec<PPackage*>     packages;
    
    PPackage* get_package(const char* name){        
        for(PPackage* pa: packages){
            if(pa->name == name){
                return pa;
            }
        }

        return nullptr;
    }

    Sym* SymImpl::find(const char* name){        
        if(not this->self){
            printf("SymImpl self == nullptr when trying to find %s.\n", name);
            exit(1);
        }

        name = Core::cstr(strf("%s_impl_%s", this->self->name, name));
        for(Sym* node: this->body){                        
            if( node->name == name) return node;            
        }
        return nullptr;
    }
    void SymImpl::push(Sym* sym){
        this->body.push(sym);
    }
    
    void final_ast_push(Decl* d){
        final_ast.push(d);
    }
    Operand operand_lvalue(Type *type, Val val){
        return {
            .type = type,
            .is_lvalue = true,
            .val = val                        
        };
    }
    Operand operand_rvalue(Type *type){
        return {
            .type = type,
            .is_lvalue = false,                                    
        };
    }
    Sym* sym_get(const char* &name){
        name = Core::cstr(name);
        for(auto& sym: Syms){
            if(sym->name == name) return sym;
        }
        for(auto& sym: localSyms){
            if(sym->name == name) return sym;                            
        }
        return nullptr;
    }
    Sym* sym_init(SymKind kind){
        Sym* s  = arena_alloc<Sym>();        
        s->type = Utils::type_unresolved();
        s->kind = kind;        
        s->impls.self = s;
        return s;
    }
    Sym* sym_var(const char* name, Type* type, Expr* expr){
        Sym* s = sym_init(SYM_VAR);
        s->name = name;
        s->type = type;
        s->expr = expr;
        return s;
    }
    Sym* sym_type(const char* name, Type* type){
        Sym* s = sym_init(SYM_TYPE);
        s->name = Core::cstr(name);
        s->type = type;        
        return s;
    }
    Sym* sym_new(const char* name, Decl* decl){
        assert(decl);
        if(Sym* sym = sym_get(decl->name)){
            if(sym->decl->kind == Ast::DECL_IMPL){                
                for(Decl* node: decl->impl.body){
                    sym->decl->impl.body.push(node);
                }

                return sym;
            }
            else {
                printf("[ERROR]: the name '%s' is already declared.\n", decl->name);
                exit(1);
            }
        }
        Sym* sym    = arena_alloc<Sym>();
        sym->decl   = decl;
        sym->name   = Core::cstr(name);
        sym->state  = SYM_UNRESOLVED;
        sym->type   = type_unresolved();

        switch(decl->kind){
            case Ast::DECL_CONSTEXPR: {
                sym->kind = SYM_CONSTEXPR;
            }
            case Ast::DECL_ENUM: {
                sym->kind = SYM_ENUM;                
                break;
            }
            case Ast::DECL_VAR:
                sym->kind = SYM_VAR;
                sym->type = resolve_typespec(decl->var.type);
                break;
            case Ast::DECL_TYPE:    {
                sym->kind           = SYM_TYPE;
                const char* name    = decl->name;
                Type* type          = resolve_typespec(decl->type_alias.type);
                CBridge::type_set(name, type);
                sym->type = type_void();
                break;
            }
            case DECL_PROC: {
                sym->kind = SYM_PROC;
                SVec<TypeField*>    params;
                for(ProcParam* param: decl->proc.params){
                    Type* ty = resolve_typespec(param->type);
                    params.push(Utils::init_typefield(param->name, ty));
                }
                Type*               ret = resolve_typespec(decl->proc.ret);

                // Global scoped procedure
                sym->type = type_proc(sym->name, params, ret, decl->proc.is_vararg, true);

                break;
            }
            case Ast::DECL_AGGREGATE:   {
                sym->kind = SYM_AGGREGATE;
                SVec<TypeField*> fields;
                size_t size = 0;

                bool isUnion = decl->aggregate.aggkind == Ast::AGG_UNION;
                if(!isUnion){
                    assert(decl->aggregate.aggkind == Ast::AGG_STRUCT);
                }

                for(AggregateItem* item: decl->aggregate.items){
                    
                    switch(item->kind){
                        case Ast::AGGREGATEITEM_FIELD:  {
                            resolve_typespec(item->field.type);
                            for(const char* name: item->field.names){
                                TypeField* field = init_typefield(name, item->field.type->resolvedTy);
                                if(isUnion){
                                    // If the type of the field is greater than the union size
                                    // We set the union size to the field size
                                    size = (size < field->type->size)? field->type->size: size;                                    
                                }
                                else {
                                    // If it is a struct we just sum all the field size components
                                    size += field->type->size;
                                }
                                fields.push(field);                                
                            }
                        } break;
                            
                                                        
                        default:
                            assert(0 && "undefined aggregate type.");                            
                    }
                }
                
                sym->type = type_aggregate(fields, decl->aggregate.aggkind != Ast::AGG_UNION);                                
                sym->type->name = sym->name;
                sym->type->size = size;
                CBridge::type_set(sym->name, sym->type);                
                break;
            }

        }
        sym->impls.self = sym;
        return sym;
    }

    void declare_ast(SVec<Decl*> ast){
        for(Decl* decl: ast){                                                
            if(decl->name == Core::cstr("<use>")){
                assert(decl->kind == DECL_USE);
                if(decl->use.rename){
                    printf("[ERROR]: decl-Use with rename..\n");
                    exit(1);
                }

                else {
                    declare_ast(decl->use.module);
                }
            }
            else {                                
                Sym* sym = sym_new(decl->name, decl);                            
                Syms.push(sym);                
            }
        }
    }
    void declare_package(PPackage* package){
        declare_ast(package->ast);
    }

    #define DEFINE_BUILTIN(_name, ...)                      \
        {                                                   \
            Syms.push(sym_new(Core::cstr(_name), __VA_ARGS__));         \
        }

    void declare_built_in(){
        DEFINE_BUILTIN("true",
            Utils::decl_var("true", 
                Utils::typespec_name("i64", {}),
                Utils::expr_int(1)));



        struct Builin_Sym_type{
            const char* name;
            Type*       type;
        };
        
        Builin_Sym_type types[]{
            {"i8", type_int(8, true)},
            {"i16", type_int(16, true)},
            {"i32", type_int(32, true)},
            {"i64", type_int(64, true)},            
            {"f32", type_float(32, true)},
            {"f64", type_float(64, true)},
            {"any", type_any()}
            
        };
        for(auto& [name, type]: types){
            Sym* _ty = sym_type(name, type);
            Syms.push(_ty);
        }
        
        

        DEFINE_BUILTIN("false",
            Utils::decl_var("false", 
                Utils::typespec_name("i64", {}),
                Utils::expr_int(0)));

        DEFINE_BUILTIN("null",
            Utils::decl_var("null", 
                Utils::typespec_name("i64", {}),
                Utils::expr_int(0)));
        {
            const char* builtin_procs[] = {
                "dump",
                "syscall",
                "asm",                
                "quit",
                "readFile", 
                "typeof",
                "sizeof",
                "va_next",
                "va_begin",
                "va_end",
                
            };
            for(auto& str: builtin_procs){
                DEFINE_BUILTIN(str,
                    Utils::decl_proc(
                        str,
                        {},
                        Utils::typespec_name("any", {}),
                        {},
                        false,
                        true,
                        {},
                        true                                                
                    ));
            }
        }
    }
    #undef DEFINE_BUILTIN

    TypeSpec* decay_typespec(TypeSpec* ts){    
        if(ts->kind == Pietra::Ast::TYPESPEC_CONST or ts->kind == Pietra::Ast::TYPESPEC_POINTER){
            return ts->base;
        }    
        return ts;
    }

    Type* resolve_typespec(TypeSpec* &ts){    
        if(!ts){
            ts = Utils::typespec_name("any", {});
            return resolve_typespec(ts);
        }

        switch(ts->kind){
            case TYPESPEC_NAME:
            #define TS_CHECK_TYPE(__ty, ...)                        \
                {                                                   \
                    Type* t = __ty(__VA_ARGS__);                    \
                    if(ts->name == t->name){                        \
                        ts->resolvedTy = t;                         \
                        return t;                                   \
                    }                                               \
                }

            #define TS_CHECK_INT(__size)                                        \
                t = type_int(__size, ts->mutablity);                            \
                if(ts->name == t->name) {                                       \
                    ts->resolvedTy = t;                                         \
                    return t;                                                   \
                }                                                               \
                
                
                Type* t;
                TS_CHECK_INT(8);
                TS_CHECK_INT(16);
                TS_CHECK_INT(32);
                TS_CHECK_INT(64);
                TS_CHECK_TYPE(type_void);
                TS_CHECK_TYPE(type_any);
                
                
                if(ts->name == type_self(CBridge::tmp_self->ismut)->name){                      
                    if(!CBridge::tmp_self){
                        printf("[ERROR]: using self outside impl context.\n");
                        exit(1);
                    }
                    ts->resolvedTy = CBridge::tmp_self;                    
                    return t;
                }
                
                if(Type* alias = CBridge::type_get(ts->name)){
                    ts->resolvedTy = alias;
                    return t;
                }
                

                if(Sym* e = sym_get(ts->name)){
                    if(e->kind == SYM_ENUM){
                        ts->resolvedTy = type_int(64, false);
                        return t;
                    }
                    else {
                      printf("[WARN]: unknown type : %s\n", e->name);
                      exit(1);
                    }
                }

                tok_err(ts->token, "Undefined type: %s\n", ts->name);                
                exit(1);
                
            #undef CHECK_INT
            case TYPESPEC_POINTER:  {
                Type* base = resolve_typespec(ts->base);
                ts->resolvedTy = type_ptr(base, ts->mutablity);
                break;
            }    
            case TYPESPEC_PROC: {
                SVec<TypeField*> tf;
                Type* ret;
                int paramId{0};
                for(TypeSpec* param: ts->proc.params){
                    param->resolvedTy = resolve_typespec(param);
                    std::string cxx_string_template = "v" + std::to_string(paramId++);
                    const char* paramName = Core::cstr(cxx_string_template.c_str());
                    tf.push(init_typefield(paramName, param->resolvedTy));
                }
                if(ts->proc.ret){
                    ts->proc.ret->resolvedTy = resolve_typespec(ts->proc.ret);
                    ret = ts->proc.ret->resolvedTy;
                }
                else {
                    ret = type_any();
                }

                
                ts->resolvedTy = type_proc("unamed_proc", tf, ret, ts->proc.has_varags, false);
                break;
            }
            case TYPESPEC_NONE:        

            case TYPESPEC_ARRAY:            
            case TYPESPEC_CONST:
            case TYPESPEC_TEMPLATE:
            default: assert(0);
        }
        assert(ts->resolvedTy);
        return ts->resolvedTy;
    }
    Operand resolve_var_init(const char* &name, TypeSpec* &type, Expr* &init, bool isLocal, bool isParam){                        
        Type* ty = resolve_typespec(type);                         
        if(init){                        
            //*init = *PreprocessExpr::expr(init);
            Operand rhs = resolve_expr(init);            
            if(!ty->typeCheck(rhs.type)){                 
                if(ty->name){                    
                    if(Sym* sym = sym_get(ty->name)){                        
                        if(Sym* __eq = sym->impls.find("__eq__")){                            
                            Type* ty = resolve_typespec(__eq->decl->proc.params.at(1)->type);
                            if(ty->typeCheck(rhs.type)){                                                            
                                goto typeCheckOk;
                            }
                        }
                    }
                }
                printf("[ERROR]: tring to assign the %s variable (%s:%s) to the type %s.\n",
                    isParam? "parameter": isLocal? "local":"global",
                    name,
                    ty->repr(),
                    rhs.type->repr()
                );
                exit(1);
            }            
        }                
        typeCheckOk:
        
        if(isLocal or isParam){            
            Sym* s = sym_var(name, type->resolvedTy, init);                        
            assert(s);
            localSyms.push(s);
            CBridge::CreateVar(
                name,
                type->resolvedTy,
                init,
                false,
                isParam
            );
        
        }
        else {
            CBridge::CreateVar(
                name,
                type->resolvedTy,
                init,
                true,
                false
            );
        }        
        return operand_rvalue(type->resolvedTy);
    }
    Operand resolve_name(const char* name){                
        Sym* sym = sym_get(name);                        
        if(!sym){
            printf("[ERROR]: the name '%s' is not declared in this scope.\n", name);
            exit(1);
        }        
        
        if(Type* type = CBridge::type_get(name)){
            return operand_lvalue(type, {0});            
        }        
        if(CBridge::CConstexpr* ce = CBridge::find_constexpr(sym->name)){
            return resolve_expr(ce->expr);
        }  
              
        else {
            resolve_sym(sym);                
        }            
        return operand_rvalue(sym->type);        
    }
    Operand resolve_call(Expr* base, SVec<Expr*> &args){                        
        Operand bop = resolve_expr(base);        
        assert(bop.type);
        if(!bop.type->isCallable()){
            printf("[ERROR]: trying to call a non-callable object %s\n", bop.type->repr());
            exit(1);
        }        
        int asz = args.len();
        int psz = bop.type->proc.params.len();    
        // TODO: type check the function and the callee arguments|parameters
        if(not bop.type->proc.is_vararg){                               
            if(bop.type->kind == TYPE_PROC and psz > 0){                                                         
                    TypeField* tf = bop.type->proc.params.at(0);                                         
                    if(tf->name == Core::cstr("self")){
                        assert(base->kind == Ast::EXPR_FIELD);                    
                        SVec<Expr*> new_args;
                        Expr* self = base->field_acc.parent;
                        Operand base_parent_op = resolve_expr(base->field_acc.parent);                    

                        if(base_parent_op.type->kind != TYPE_PTR){
                            // if self is not a poiner, then we make self be &self
                            self = Utils::expr_unary(Lexer::TK_AMPERSAND, self);
                        }
                        assert(self);
                        new_args.push(self);
                        for(Expr* arg: args) {
                            resolve_expr(arg);
                            new_args.push(arg);
                        }
                        args = new_args;              
                        asz = args.len();                    
                    }                                    
            }                
            if(asz != psz){
                printf("[ERROR]: Got the wrong amount of arguments when calling an implementation procedure: %s\n", 
                    bop.type->repr());
                printf("[NOTE]: expected %i got %i\n",
                    psz, asz);
                exit(1);
            }                                                        
        }               
        

        Type* ret = bop.type->proc.ret_type;                        
        return operand_rvalue(ret);
        
    }
    Operand resolve_unary(Lexer::tokenKind kind, Expr* expr){
        switch(kind){
            case Lexer::TK_NOT: {
                return resolve_expr(expr);
            }
            case Lexer::TK_AMPERSAND:   {
                Operand op = resolve_expr(expr);
                if(op.is_lvalue){
                    printf("[ERROR]: trying to get the address if an lvalue.\n");
                    exit(1);
                }

                op.type = type_ptr(op.type, op.type->ismut);
                return op;
            }

            case Lexer::TK_SUB: {
                return resolve_expr(expr);
            }

            case Lexer::TK_MULT:    {
                // derrefencing
                Operand op = resolve_expr(expr);
                if(op.is_lvalue){
                    printf("[ERROR: trying to derreference an lvalue.\n");
                    exit(1);
                }
                if(op.type->kind != TYPE_PTR){
                    printf("[ERROR]: trying to derreference a non-pointer type: %s\n", op.type->repr());
                    exit(1);
                }

                op.type = op.type->base;
                return op;
            }
            default: assert(0);
        }
    }
    static inline bool is_assign(Lexer::tokenKind kind){
        return kind == Lexer::TK_EQ;
    }
    Operand resolve_assign(Expr* base, Expr* rhs){
        Operand b = resolve_expr(base);        
        Operand r = resolve_expr(rhs);
        

        
        // FIXME: ismut is not being passed to the resolver stage.
        if(not b.type->ismut){            
            printf("[ERROR]: trying to modify an imutable type: %s (", b.type->repr());                        
            pPrint::expr(base);
            printf(")\n");
            exit(1);
        }

        if(!b.type->typeCheck(r.type)){
            if(Sym* sym = sym_get(b.type->name)){
                if(Sym* __eq = sym->impls.find("__eq__")){
                    if(ProcParam* param = __eq->decl->proc.params.at(1)){
                        Type* ty = resolve_typespec(param->type);
                        if(ty->typeCheck(r.type)){
                            return b;
                        }
                    }
                }
            }
            
            printf("[ERROR]: trying to assign a type %s to %s\n",
                b.type->repr(),
                r.type->repr());
            exit(1);            
            
        }
        return b;
    }
    Operand resolve_binary(Lexer::tokenKind kind, Expr* lhs, Expr* rhs){
        if(is_assign(kind)){
            return resolve_assign(lhs, rhs);
        }
        switch(kind){            
            case Lexer::TK_LAND:
            case Lexer::TK_LOR:
            case Lexer::TK_PIPE:    {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);
                return l;
            }
            case Lexer::TK_NEQ:
            case Lexer::TK_CMPEQ:   {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                return operand_lvalue(type_int(8, l.type->ismut), {});                
            }
            case Lexer::TK_SUB: {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                if(l.type->kind == r.type->kind and l.type->kind == TYPE_PTR){
                    return operand_lvalue(type_int(64, l.type->ismut), {});
                }

                return l;
            }            
            case Lexer::TK_DIV: {
                Operand l = resolve_expr(lhs);                
                Operand r = resolve_expr(rhs);

                if(r.is_lvalue){
                    if(r.val.i == 0){
                        printf("[WARN]: trying to divide for zero, the compiler allows it. but may cause a segfault.\n");                        
                    }
                }

                return l;
            }
            case Lexer::TK_GT:
            case Lexer::TK_MULT:
            case Lexer::TK_LT:
            case Lexer::TK_LTE:            
            case Lexer::TK_MOD:
            case Lexer::TK_ADD: {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                return l;
            }
            default: assert(0);
        }
    }
    Operand resolve_index(Expr* &expr){
        Expr* base  = expr->array.base;
        Expr* index = expr->array.index;

        //*index = *PreprocessExpr::expr(index);

        // base[index] -> *(base + sizeof(base)+index)
        Operand bop = resolve_expr(base);
        if(bop.type->kind != TYPE_PTR){
            printf("[ERROR]: trying to get the index of a non-pointer object.\n");
            exit(1);
        }
        Type* b_t = bop.type->base;
        
        expr = Utils::expr_unary(Lexer::TK_MULT, expr); 
        
        Expr* offset = Utils::expr_binary(Lexer::TK_MULT, index, Utils::expr_int(b_t->size));
        Expr* item = Utils::expr_binary(Lexer::TK_ADD, base, offset);

        expr = item; 
        
        
        return operand_rvalue(b_t);
        

    }
    Operand resolve_cast(TypeSpec* typespec, Expr* expr){
        resolve_typespec(typespec);
        assert(typespec->resolvedTy);

        Operand op = resolve_expr(expr);
        op.type = typespec->resolvedTy;
        return op;
    }
    Operand resolve_field(Expr* parent, Expr* children){        
        if(children->kind != Ast::EXPR_NAME){
            printf("[ERROR]: idk kys.\n");
            exit(1);
        }

        if(Sym* s = sym_get(parent->name)){
            if(s->kind == SYM_ENUM){                
                if(Sym* field = s->impls.find(children->name)){                
                    return operand_lvalue(type_int(64, s->type->ismut), {0});
                }
            }            
        }

        Operand op = resolve_expr(parent);                
        if(op.type->kind == TYPE_PTR){
            if(op.type->base->kind == Ast::TYPE_STRUCT or op.type->base->kind == Ast::TYPE_UNION){
                op.type = op.type->base;
            }
        }        
        // Check for TypeImpl in op.type               
        if(op.type->name){
            if(Sym* sym = sym_get(op.type->name)){                                        
                if(sym->state == SYM_UNRESOLVED){
                    resolve_sym(sym);
                }                        
                SymImpl& impls = sym->impls;                
                if(impls.self){
                    //const char* target = Core::cstr(strf("%s_impl_%s", impls.self->name, children->name));                
                    if(Sym* impl = impls.find(children->name)){
                        assert(impl->type->kind == TYPE_PROC);                    
                        return operand_rvalue(impl->type);
                    }
                }
            }
        }
        if(op.type->kind == TYPE_STRUCT or op.type->kind == TYPE_UNION){                                
            for(TypeField* field: op.type->aggregate.items){
                if(field->name != children->name){
                    continue;
                }                                
                return operand_rvalue(field->type);
            }            
        }                   
        
        
        printf("[ERROR]: field not found: '%s'in type %s\n", children->name, op.type->repr());
        exit(1);
        
    }
    Operand resolve_ternary(Expr* cond, Expr*  if_case, Expr* else_case){
        // NOTE: ternary will never be mutable
        return operand_lvalue(type_int(64, false), {0});
    }
    Operand resolve_expr_switch(Expr* e){
        assert(e->kind == Ast::EXPR_SWITCH);
        Expr* cond = e->expr_switch.cond;                
        SVec<SwitchCase*> cases = e->expr_switch.cases;
        SVec<Stmt*> default_case = e->expr_switch.default_case;            
        auto has_default = e->expr_switch.has_default;

        resolve_expr(cond);
        if(has_default){
            resolve_stmt_block(default_case);
        }

        Operand ret;
        bool ret_initialized = false;
        for(SwitchCase*& c: cases){
            for(Stmt*& st: c->block){                

                if(st->kind != STMT_EXPR){
                    printf("[ERROR]: switch case in expressions must hold only expressions.\n");
                    exit(1);
                }

                Operand got = resolve_expr(st->expr);
                if(ret_initialized){
                    if(not got.type->typeCheck(got.type)){
                        printf("[ERROR]: The expressions inside the switch cases must return the same type.\n");
                        exit(1);
                    }
                }

                ret = got;
                ret_initialized = true;
            }
        }

        return ret;
    }
    Operand resolve_expr(Expr* &expr){        
        if(!expr){
            return operand_lvalue(type_void(), {0});
        }

        printf("Resolving expr: ");
        pPrint::expr(expr);
        printf("\n");
        switch(expr->kind){
            case EXPR_INT:          return operand_lvalue(type_int(64, true), {.ul = expr->int_lit});
            case EXPR_STRING:       return operand_lvalue(type_string(false), {});
            case EXPR_NAME:         
                // NOTE: self is a parameter, so it shall be resolved 
                if(expr->name == Core::cstr("self")){
                  // DEBUG PORPUSE
                  assert(CBridge::tmp_self && "tmp_self == nullptr");
                  Sym* sym = sym_get(expr->name);
                  assert(sym && "self::sym == nullptr");
                  printf("RETURNING SELF\n");;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
                  return operand_rvalue(sym->type);
                }
                return resolve_name(expr->name);
            case EXPR_CALL: {
                if(expr->call.base->kind == EXPR_NAME){
                    if(expr->call.base->name == Core::cstr("typeof")){
                        if(expr->call.args.len() != 1){
                            printf("[ERROR]: typeof expects one argument.\n");
                            exit(1);
                        }
                        Operand arg = resolve_expr(expr->call.args.at(0));
                        const char* repr = arg.type->repr();                        
                        expr = Utils::expr_string(repr);
                        return operand_lvalue(type_string(false), {0});
                    }
                }
                return resolve_call(expr->call.base, expr->call.args);            
            }
            case EXPR_INIT_VAR:     return resolve_var_init(expr->init_var.name, expr->init_var.type, expr->init_var.rhs, true, false);
            case EXPR_UNARY:        return resolve_unary(expr->unary.unary_kind, expr->unary.expr);
            case EXPR_BINARY:   {
                if(expr->binary.binary_kind == Lexer::TK_LTE){
                    // T: lhs <= rhs --> not(lhs > rhs);                    
                    Expr* lhs = expr->binary.left;
                    Expr* rhs = expr->binary.right;
                    Expr* base_gt = Utils::expr_binary(Lexer::TK_GT, lhs, rhs);
                    expr = Utils::expr_unary(Lexer::TK_NOT, base_gt);
                    return resolve_expr(expr);                    
                }
                else if (expr->binary.binary_kind == Lexer::TK_NEQ){
                    // T: lhs != rhs --> not(lhs == rhs);                    
                    Expr* lhs = expr->binary.left;
                    Expr* rhs = expr->binary.right;
                    Expr* base_eq = Utils::expr_binary(Lexer::TK_CMPEQ, lhs, rhs);                    
                    expr = Utils::expr_unary(Lexer::TK_NOT, base_eq);                    
                    return resolve_expr(expr);                    
                }
                else {
                    return resolve_binary(expr->binary.binary_kind, expr->binary.left, expr->binary.right);
                }
            }            
            case EXPR_SWITCH:       return resolve_expr_switch(expr);
            case EXPR_ARRAY_INDEX:  return resolve_index(expr);
            case EXPR_CAST:         return resolve_cast(expr->cast.typespec, expr->cast.expr);
            case EXPR_FIELD:        return resolve_field(expr->field_acc.parent, expr->field_acc.children);
            case EXPR_TERNARY:      return resolve_ternary(expr->ternary.cond, expr->ternary.if_case, expr->ternary.else_case);
        }        

        printf("ERROR AT RESOLVE EXPR.\n");
        pPrint::expr(expr);
        exit(1);
    }

    void resolve_stmt(Stmt* &stmt){
        switch(stmt->kind){
            case Ast::STMT_EXPR:
                resolve_expr(stmt->expr);
                break;
            case Ast::STMT_IF:
                resolve_expr(stmt->stmt_if.if_clause->cond);
                resolve_stmt_block(stmt->stmt_if.if_clause->block);
                break;
            case Ast::STMT_WHILE:   {
                resolve_expr(stmt->stmt_while->cond);
                resolve_stmt_block(stmt->stmt_while->block);
                break;
            }
            case Ast::STMT_RETURN: {                
                if(!CBridge::ctx.Cp){
                    printf("wtf where is CBridge::ctx.Cp?\n");
                    exit(1);                
                }
                Operand got         = resolve_expr(stmt->expr);                
                Type*   expected    = CBridge::ctx.Cp->type->proc.ret_type;                

                if(!expected->typeCheck(got.type)){
                    printf("[ERROR]: expected %s in the return of the procedure %s, but got: %s\n",
                        expected->repr(),
                        CBridge::ctx.Cp->name,
                        got.type->repr()
                    );
                    exit(1);
                }
                
                break;
            }
            case Ast::STMT_SWITCH: {
                resolve_expr(stmt->stmt_switch.cond);
                for(SwitchCase* c: stmt->stmt_switch.cases){
                    resolve_stmt_block(c->block);
                }
                if(stmt->stmt_switch.has_default){
                    resolve_stmt_block(stmt->stmt_switch.default_case);
                }
                break;
            }
            default: assert(0);
        }
    }

    void resolve_stmt_block(SVec<Stmt*> block){
        for(Stmt* stmt: block) 
            resolve_stmt(stmt);
    }
    void resolve_sym_proc_impl(Sym* self, Sym* sym){
        assert(sym->kind == SYM_PROC);        
        self->impls.self = self;                
        for(ProcParam* pp: sym->decl->proc.params){
            if(pp->name == Core::cstr("self")){
                if(self->type->size == 0){                    
                    if(!find_note(sym->decl->notes, "unsafe")){
                        printf("[ERROR]: can't use self in zero-sized in the struct %s. \n--> %s.\n",
                            self->type->repr(),
                            sym->type->repr()
                        );
                        exit(1);
                    }
                }
            }
        }
        resolve_decl_proc(sym->decl, sym->type);
    }
    void resolve_impl(const char* target, SVec<Decl*> body){        
        Sym* sym = sym_get(target);
        if(!sym){
            printf("[ERROR]: could not find the target of the implementation: %s.\n", target);
            exit(1);
        }
        if(sym->kind != SYM_TYPE and sym->kind != SYM_AGGREGATE){
            printf("[ERROR]: impl can only work with objects or types.\n");
            exit(1);
        }
        SymImpl& impls = sym->impls;
        impls.self = sym;        
        CBridge::tmp_self = sym->type;
        sym->type->isSelf = true;
        impl_ctx = true;
        for(Decl* node: body){
            node->name = Core::cstr(strf("%s_impl_%s", sym->name, node->name));
            if(impls.find(node->name)){
                printf("[ERROR]: duplicated field in impl (%s.%s).\n", sym->name, node->name);
                exit(1);
            }
            
            // Make a symbol for the declaration
            {                
                Sym* snode = sym_new(node->name, node);
                if(snode->kind != SYM_PROC){
                    printf("[ERROR]: for now impl only accept procedures.\n");
                    exit(1);
                }
                
                impls.push(snode);
                resolve_sym_proc_impl(sym, snode);                                
            }                        
        }
        CBridge::tmp_self = nullptr;                
        impl_ctx = false;
    }
    void resolve_decl_var(Decl* &d){
        resolve_var_init(d->name, d->var.type, d->var.init, false, false);        
    }
    void resolve_decl_proc(Decl* &d, Type* type){                
        int i = 0;

        printf("at = %i\n", i++);
        if(d->notes.len() > 0){
        printf("at = %i\n", i++);
            SVec<Note*>& notes = d->notes;
            for(Note* note: notes){
                if(note->name == Core::cstr("todo")){
                    printf("[TODO]: %s\n", d->name);
                    for(Expr* arg: note->args){
                        if(arg->kind != EXPR_STRING){
                            printf("[INTERNAL-ERROR]: @todo expects only string inside it's arguments.\n");
                            exit(1);
                        }
                        printf("\t%s\n", arg->string_lit);
                    }
                }
                else if(note->name == Core::cstr("unsafe")){} // Just ignore
                else if(note->name == Core::cstr("string_comparator")){
                    if(impl_ctx){
                        printf("[ERROR]: @string_comparator can't be used inside a impl method.\n");
                        exit(1);
                    }
                    string_comparator = d;    
                }
                else {
                    show_all_decorators();
                    printf("[ERROR]: compiler doesn't understand the decl note %s\n", note->name);
                    exit(1);
                }
            }
        }
        assert(d->kind == DECL_PROC);
        localSyms.free();        
        CBridge::CProc* proc = CBridge::CreateProc(d->name, type);
                
        for(ProcParam* pp: d->proc.params){            
            resolve_var_init(pp->name, pp->type, pp->init, false, true);
        }        

        printf("at = %i\n", i++);
        resolve_stmt_block(d->proc.block);
        printf("at = %i\n", i++);

        
    }
    void resolve_decl_use(Decl* &decl){        
        assert(decl->kind == DECL_USE);

        if(decl->use.rename){            
            PPackage* package = arena_alloc<PPackage>();
            package->ast    = decl->use.module;
            package->name   = decl->use.rename;            
            packages.push(package);

            for(auto& f: package->ast){
                printf("IN PACKAGE %s got %s\n", package->name, f->name);
            }
            
        }
        else {        
            for(Decl* node: decl->use.module){
                Sym* sym = sym_new(node->name, node);
                resolve_sym(sym);            
            }
        }
    }
    void resolve_decl_enum(Decl* d){
        assert(d->kind == Ast::DECL_ENUM);
        Sym* e = sym_get(d->name);        
        assert(e);        
        SymImpl& impls = e->impls;
        int count = 0;
        for(EnumItem* item: d->enumeration.items){
            if(item->init){
                *item->init = *PreprocessExpr::expr(item->init);
                if(item->init->kind != EXPR_INT){
                    printf("[ERROR]: expected initializer in enumeration item to be an integer.\n");
                    exit(1);
                }
                count = item->init->int_lit;
            }            
            item->name = Core::cstr(strf("%s_impl_%s", d->name, item->name));
            Sym* item_sym = sym_new(item->name, Utils::decl_constexpr(item->name, Utils::expr_int(count)));            
            impls.body.push(item_sym);
            count++;
        }                        
    }
    void resolve_decl(Decl* &decl, Type* type){
        switch(decl->kind){
            case DECL_VAR:
                resolve_decl_var(decl);
                break;                
            case DECL_PROC:
                resolve_decl_proc(decl, type);
                break;
            case DECL_CONSTEXPR: 
                CBridge::save_constexpr(decl->name, PreprocessExpr::expr(decl->expr));
                break;                 
            case DECL_USE:
                resolve_decl_use(decl);
                break;
            case DECL_AGGREGATE:    break;
            case DECL_TYPE:         break;
            case DECL_IMPL: 
                resolve_impl(decl->impl.target, decl->impl.body);
                break;
            case DECL_ENUM:
                resolve_decl_enum(decl)            ;
                break;
            case DECL_NONE:             
            
            default:
                printf("[ERR]: Resolving unimplemented for:\n");
                pPrint::decl(decl);                        
        }

    }
    Sym* resolve_sym(Sym* sym){
        assert(sym);
        if(sym->state == SYM_RESOLVED) return sym;
        if(sym->state == SYM_RESOLVING){
            printf("Cyclic dependence at: %s.\n", sym->name);
            exit(1);
        }
        printf("resolving_sym(%s: %s)\n", sym->name, sym->type? sym->type->repr(): "<>");
        sym->state = SYM_RESOLVED;
        if(sym->decl){
            if(sym->decl){ 
                if(sym->decl->kind == DECL_PROC and sym->decl->proc.params.len() > 0){                
                    if(sym->decl->proc.params.at(0)->name == Core::cstr("self")){                    
                        printf("[ERROR]: The parameter named self is reserved to be inside implementation context.\n");
                        exit(1);
                    }
                }
            // some syms are expressions            
            resolve_decl(sym->decl, sym->type);
            }
            final_ast_push(sym->decl);
        }
        sym->state = SYM_RESOLVED;
        return sym;
    }
    
    SVec<Decl*> resolve_package(PPackage* &package){                                                  
        declare_package(package);

        for(Sym* sym: Syms){
            static int i = 0;
            printf("Resolving[%i: AST_NODE: %s]\n", i++, sym->name);
            resolve_sym(sym);        
        }   
        
        
        return final_ast;        
    }    
}
#endif /*RESOLVE_CPP*/
