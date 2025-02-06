/*
The resolver will
[1]: order the AST
[2]: TODO: type checking
[3]: TODO: const evaluation    
*/
#ifndef RESOLVE_CPP
#define RESOLVE_CPP
#include "../include/resolve.hpp"
#include "./context.cpp"
#include "ast.cpp"
#include "pprint.cpp"
#define STRUCT_REASSIGN_FLAG    cstr("struct_reassign")
#define NEW_FLAG                cstr("new_allocator")


static int  scope_lvl = 0;
bool        impl_ctx = false;
Decl*       string_comparator = nullptr;
Decl*       struct_reassigner = nullptr;
Decl*       new_allocator     = nullptr;
SVec<Resolver::Sym*>  Declared_lambdas;
#define ASSERT_PTR(_ptr) _ptr; assert(_ptr);

void allocate_lambda(Resolver::Sym* lambda){
    Declared_lambdas.push(lambda);    
}

void Resolver::Scope::add_sym(Resolver::Sym* sym) {            
    m_symbols.push_back(sym);
}




Resolver::Sym* Resolver::Scope::find_sym(const char* name) {        
    for (auto sym : m_symbols) {
        if (sym->name == name) {            
            return sym;
        }
    }
    return nullptr;
}

void Resolver::ScopeManager::enter_scope() {
    resolver_log({}, "-- ENTERED A NEW SCOPE\n");
    m_scopes.push_back(new Scope);
}
void Resolver::ScopeManager::print_all(){
    if(this->m_scopes.empty()) return;
    

    for(Scope* scope: this->m_scopes){
        printf("Entering scope:\n");
        for(Sym* symbol: scope->m_symbols){
            resolver_log({}, "symbol - %s\n", symbol->name);                        
        }
        
    }
}
void Resolver::ScopeManager::leave_scope() {
    if (!this->m_scopes.empty()) {
        this->m_scopes.pop_back();        
    }
}
int Resolver::ScopeManager::add_sym(Resolver::Sym* sym) {    
    if (!m_scopes.empty()) {        
        //return 0;
    }   
    this->m_scopes.back()->add_sym(sym); 
    return 0;
    printf("[ERROR]: m_scopes is empty failed to push symbol: sz = %zu.\n", this->m_scopes.size());
    exit(1);
}
Resolver::Sym* Resolver::ScopeManager::find_sym(const char* name) {    
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {                
        Scope* scope = *it;
        assert(scope);
        if (Resolver::Sym* sym = scope->find_sym(name)) {    
            return sym;
        }
    }
    return nullptr;
}

Note* find_note(SVec<Note*> notes, const char* name){    
    name = Core::cstr(name);    
    for(Note* note: notes){
        if(note->name == name) return note;
    }

    return nullptr;
}
#define LIST_SIZEOF(__list)     (sizeof((__list)) / sizeof((__list)[0]))
#define LIST_LAST_ELEM(__list)  (__list)[LIST_SIZEOF(__list) - 1]
#define SAD_printf_cond(item, __list) ((item) == LIST_LAST_ELEM(__list))? "": ", " 
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
        "todo", "inline", "error", "warn", "string_comparator", "flag", "method_of"
    };
            
    printf("Decorators = [");
    for(const char* dc: decorators_name){
        printf("@%s%s", 
            dc, 
            SAD_printf_cond(dc, decorators_name));
    }
    printf("]\n");  
}
namespace Pietra::Resolver{        
    SVec<Sym*>          global_syms;            
    SVec<Decl*>         resolved_ast;
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
    
    
    
    Operand operand_lvalue(Type *type, Val val){
        return {
            .type = type,
            .is_lvalue = true,
            .val = val                        
        };
    }
    Operand operand_none(){        
        return {
            .type       = type_none(),
            .is_lvalue  = false,
            .m_isNone   = true
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
        Sym* sym = nullptr;              
        if(ctx.currentProcedure){
            sym = ctx.getLocalScope()->find_sym(name);
            if(sym) return sym;
        }
        
        sym = ctx.getGlobalScope()->find_sym(name);
        if(sym) return sym;        
        return nullptr;
    }
    Sym* sym_getlval(const char* name){
        return sym_get(name);
    }
    Sym* sym_init(SymKind kind){
        Sym* s  = arena_alloc<Sym>();        
        s->type = type_unresolved();
        s->kind = kind;        
        // useless 
        s->impls.self = s;
        return s;
    }
    Sym* sym_var(const char* name, Type* type, Expr* expr){
        Sym* s = sym_init(SYM_VAR);
        if(expr){
            if(expr->kind == Ast::EXPR_LAMBDA){
                resolve_lambda(expr, nullptr);
            }
        }
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
                assert(type);
                typedefs.addTypeDefinition(name, type);
                sym->type = type_void();
                break;
            }
            case DECL_PROC: {
                sym->kind = SYM_PROC;
                SVec<TypeField*>    params;
                for(ProcParam* param: decl->proc.params){
                    Type* ty = resolve_typespec(param->type);
                    params.push(init_typefield(param->name, ty));
                }
                Type* ret = resolve_typespec(decl->proc.ret);
                
                assert(ret);

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
                // TODO: parse struct mutablity
                sym->type = type_aggregate(fields, decl->aggregate.aggkind != Ast::AGG_UNION, true);
                sym->type->name = sym->name;
                sym->type->size = size;
                typedefs.addTypeDefinition(sym->name, sym->type);                
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
                ctx.getGlobalScope()->add_sym(sym);        
                global_syms.push(sym);                
            }
        }
    }
    void declare_package(PPackage* package){
        declare_ast(package->ast);
    }

    #define DEFINE_BUILTIN(_name, ...)                      \
        {                                                   \
            ctx.getGlobalScope()->add_sym(sym_new(Core::cstr(_name), __VA_ARGS__));         \
        }

    void declare_built_in(){               
        assert(ctx.getGlobalScope());
        ctx.getGlobalScope()->enter_scope();
        DEFINE_BUILTIN("true",
            Utils::decl_var({"<builtin>", 0}, "true", 
                Utils::typespec_name("i64", {}),
                Utils::expr_int({}, 1)));



        struct Builtin_Sym_type{
            const char* name;
            Type*       type;
        };
        
        Builtin_Sym_type types[]{
            {"i8",      type_int(8, true)},
            {"i16",     type_int(16, true)},
            {"i32",     type_int(32, true)},
            {"i64",     type_int(64, true)},            
            {"f32",     type_float(32, true)},
            {"f64",     type_float(64, true)},
            {"any",     type_any()},
            {"void",    type_any()}
        };

        for(auto& [name, type]: types){            
            Sym* _ty = sym_type(name, type);
            global_syms.push(_ty);
            ctx.getGlobalScope()->add_sym(_ty);
        }
        
        

        DEFINE_BUILTIN("false",
            Utils::decl_var({"<builtin>", 0}, "false", 
                Utils::typespec_name("i64", {}),
                Utils::expr_int({}, 0)));

        
        {
            const char* builtin_procs[] = {                
                "syscall",
                "asm",
                "sizeof",                                
            };
            for(auto& str: builtin_procs){
                DEFINE_BUILTIN(str,
                    Utils::decl_proc(
                        {"<builtin>", 0},
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

            Type* resolved_t = resolve_typespec(ts);
            resolved_t->ismut = true;
            return resolved_t;
        }

       

        switch(ts->kind){
            case TYPESPEC_NAME:
            #define TS_CHECK_TYPE(__ty, ...)                        \
                {                                                   \
                    Type* t = __ty(__VA_ARGS__);                    \
                    if(ts->name == t->name){                        \
                        ts->resolvedTy = t;                         \
                        break;                                      \
                    }                                               \
                }

            #define TS_CHECK_INT(__size)                                            \
                {                                                                   \
                    Type* t = type_int(__size, true);                               \
                    if(ts->name == t->name) {                                       \
                        ts->resolvedTy = t;                                         \
                        break;                                                      \
                    }                                                               \
                }

        
                
                TS_CHECK_INT(8);
                TS_CHECK_INT(16);
                TS_CHECK_INT(32);
                TS_CHECK_INT(64);
                TS_CHECK_TYPE(type_void);
                TS_CHECK_TYPE(type_any);
                
                if(TypeDefinitions::TypeDefinition* tf = typedefs.getTypeDefinition(ts->name)){                    
                    ts->resolvedTy = tf->type;
                    break;
                }

                if(ts->name == type_self()->name){
                    if(!ctx.getSelf()){
                        printf("[ERROR]: using Self outside Impl context.\n");
                        exit(1);
                    }
                    
                    ts->resolvedTy = ctx.getSelf();
                    break;
                }

                if(Sym* e = sym_get(ts->name)){
                    if(e->kind == SYM_ENUM){
                        return type_int(64, false);
                    }
                }
                printf("Undefined type: %s\n", ts->name);
                
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
        
        if(type->name == Core::cstr("any") and not isParam){
            if(not init){
                printf("Current proceedure %s\n", ctx.currentProcedure->name);
                printf("[ERROR]: Trying to initialize the variable `%s` as any with no initial value.\n", name);                
                exit(1);
            }


            Operand init_op = resolve_expr(init);
            Type* init_t = init_op.type;
            type->name = init_t->name;
            type->resolvedTy = init_t;
                    
            printf("[INFO]: Implicit cast in variable `%s` -> `%s`.\n", name, init_t->repr());
        }

        if(init){                        
            //*init = *PreprocessExpr::expr(init);
            Operand rhs = resolve_expr(init);            
            if(type->name == Core::cstr("auto")){
                *type->resolvedTy = *rhs.type;                
                ty = type->resolvedTy;
                type->name = ty->name;                
                printf("auto got %s\n", type->resolvedTy->repr());                
            }            

            assert(ty);
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

                resolver_error(init->loc, "[ERROR]: tring to assign the %s variable (%s:%s) to the type %s.\n",
                    isParam? "parameter": isLocal? "local":"global",
                    name,
                    ty->repr(),
                    rhs.type->repr()
                );
                exit(1);
            }            
        }                
        typeCheckOk:
        Sym* sym = sym_var(name, type->resolvedTy, init);
        if(isLocal or isParam){                                    
            assert(sym);                        
            ctx.getLocalScope()->add_sym(sym);
            //localSyms.push(s);
            Variables::Variable* var = Factory::createVariable(name, type->resolvedTy, init, false, isParam);
            ctx.allocateVariableInCurrentProcedure(var, var->isParameter);
        }
        else {
            Variables::Variable* var = Factory::createVariable(name, type->resolvedTy, init, true, false);
            ctx.allocateGlobalVariable(var);                        
            if(!ctx.getGlobalScope()->find_sym(sym->name)){
                ctx.getGlobalScope()->add_sym(sym);
            }
        }        
        return operand_rvalue(type->resolvedTy);
    }
    Operand resolve_name(const char* name){    
        Sym* sym = sym_get(name);                
        if(!sym){
            printf("Current procedure: %s\n", ctx.currentProcedure->name);
            resolver_error({"", 0}, "[ERROR]: the name '%s' is not declared in this scope.\n", name);            
            exit(1);
        }
        
        if(TypeDefinitions::TypeDefinition* tf = typedefs.getTypeDefinition(name)){
            return operand_lvalue(tf->type, {0});            
        }
                
        if(Constants::ConstantExpression* ce = ctx.getConstantExpression(sym->name)){
            return resolve_expr(ce->expr);
        }        
        
        resolve_sym(sym);                        
        return operand_rvalue(sym->type);        
    }
    Operand resolve_call(Expr* base, SVec<Expr*> &args){                                
        Operand bop = resolve_expr(base);                
        
        assert(bop.type);
        if(!bop.type->isCallable()){
            syntax_error(base->loc, "[ERROR]: trying to call a non-callable object %s\n", bop.type->repr());            
            pPrint::expr(base);
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
                            self = Utils::expr_unary(self->loc, TK_and, self);
                        }
                        assert(self);
                        new_args.push(self);
                        for(Expr* &arg: args) {
                            resolve_expr(arg);
                            new_args.push(arg);
                        }
                        args = new_args;              
                        asz = args.len();                    
                    }                                    
            }                
            if(asz != psz){
                resolver_error(base->loc, "[ERROR]: Got the wrong amount of arguments when calling an implementation procedure: %s\n", 
                    bop.type->repr());
                printf("[NOTE]: expected %i got %i\n",
                    psz, asz);
                exit(1);
            }                                                        
        }               
        

        Type* ret = bop.type->proc.ret_type;
        return operand_rvalue(ret);
        
    }
    Operand resolve_unary(TokenKind kind, Expr* expr){
        switch(kind){
            case TK_not: {
                return resolve_expr(expr);
            }
            case TK_and:   {
                Operand op = resolve_expr(expr);
                if(op.is_lvalue){
                    printf("[ERROR]: trying to get the address if an lvalue.\n");
                    exit(1);
                }

                op.type = type_ptr(op.type, op.type->ismut);
                return op;
            }

            case TK_minus: {
                return resolve_expr(expr);
            }

            case TK_mul:    {
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
    static inline bool is_assign(TokenKind kind){
        return kind == TK_eq;
    }
    Operand resolve_assign(Expr* base, Expr* rhs){
        Operand b = resolve_expr(base);        
        Operand r = resolve_expr(rhs);
        

        
        // FIXME: ismut is not being passed to the resolver stage.
        if(not b.type->ismut){            
            syntax_error(base->loc, "[ERROR]: trying to modify an imutable type: %s (", b.type->repr());                        
            pPrint::expr(base);
            printf(")\n");
            exit(1);
        }
        // TODO: wrap those stuff to specific methods
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
            
            syntax_error(base->loc, "[ERROR]: trying to assign a type %s to %s\n", b.type->repr(), r.type->repr());
            pPrint::expr(base);
            exit(1);            
            
        }
        return b;
    }
    Operand resolve_binary(TokenKind kind, Expr* lhs, Expr* rhs){
        if(is_assign(kind)){
            return resolve_assign(lhs, rhs);
        }
        switch(kind){           
            // TODO: add support in lexer for both AND &   (as bit and)
            case TK_andand:
            case TK_oror:
            case TK_or:           
            {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);
                return l;
            }
            case TK_noteq:
            case TK_eqeq:   {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                return operand_lvalue(type_int(8, l.type->ismut), {});                
            }
            case TK_minus: {
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                if(l.type->kind == r.type->kind and l.type->kind == TYPE_PTR){
                    return operand_lvalue(type_int(64, l.type->ismut), {});
                }

                return l;
            }            
            case TK_div: {
                Operand l = resolve_expr(lhs);                
                Operand r = resolve_expr(rhs);

                if(r.is_lvalue){
                    if(r.val.i == 0){
                        printf("[WARN]: trying to divide for zero, the compiler allows it. but may cause a segfault.\n");                        
                    }
                }

                return l;
            }
            case TK_mul:
            case TK_greater:
            case TK_less:
            case TK_lesseq:
            case TK_mod:
            case TK_plus:{
                Operand l = resolve_expr(lhs);
                Operand r = resolve_expr(rhs);

                return l;
            }
            default: 
                printf("Couldn't resolve this binary expression.\n");
                pPrint::expr(Utils::expr_binary(lhs->loc, kind, lhs, rhs));
                exit(1);
        }
    }
    Operand resolve_index(Expr* &expr){
        Expr* base  = expr->array.base;
        Expr* index = expr->array.index;

        //*index = *PreprocessExpr::expr(index);

        // base[index] -> *(base + sizeof(base)+index)
        Operand bop = resolve_expr(base);
        if(bop.type->kind != TYPE_PTR){
            syntax_error(expr->loc, "[ERROR]: trying to get the index of a non-pointer object (");
            pPrint::expr(base);
            printf(") of type %s\n", bop.type->repr());
            exit(1);
        }
        Type* b_t = bop.type->base;
        
        
        expr = Utils::expr_unary(expr->loc, TK_mul, expr); 
        
        Expr* offset    = Utils::expr_binary(expr->loc, TK_mul, index, Utils::expr_int(expr->loc, b_t->size));
        Expr* item      = Utils::expr_binary(expr->loc, TK_plus, base, offset);

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
    Operand resolve_field_as_enum(Expr* parent, Expr* children){
        if(parent->name and children->name){
            if(Sym* s = sym_get(parent->name)){            
                if(s->kind == SYM_ENUM){                                
                    if(Sym* field = s->impls.find(children->name)){                                    
                        return operand_lvalue(type_int(64, s->type->ismut), {0});
                    }
                }            
            }
        }        
        return operand_none();
    }
    Type* auto_derref_struct_for_field_access(Type* struct_type){
        if(struct_type->kind != TYPE_PTR) {
            return nullptr;
        }
        if (struct_type->base->kind != TYPE_STRUCT and struct_type->base->kind != TYPE_UNION){
            return nullptr;
        }

        return struct_type->base;
    }
    Operand resolve_field_type_impl(Operand op, const char* field){        
        if(!op.type->name){
            printf("[ERROR]: op.type->name == nullptr, op.type = %s;\n", op.type->repr());
            return operand_none();
        }
        if(Sym* sym = sym_get(op.type->name)){                            
            if(sym->state == SYM_UNRESOLVED){
            
                resolver_log((sym->decl) ? sym->decl->loc : sym->expr->loc, "unresolved sym ini field.\n");
                resolve_sym(sym);
            }                        
            SymImpl* impls = &sym->impls;                  
            if(impls->self){
                //const char* target = Core::cstr(strf("%s_impl_%s", impls.self->name, children->name));                
                if(Sym* impl = impls->find(field)){
                    assert(impl->type->kind == TYPE_PROC);                    
                    
                    return operand_rvalue(impl->type);
                }
            }            
        }                                      
        return operand_none();
    }
    Operand get_aggregate_field(Operand op, const char* field){
        if(op.type->kind == TYPE_STRUCT or op.type->kind == TYPE_UNION){                                
            for(TypeField* aggregate_item: op.type->aggregate.items){
                if(aggregate_item->name != field){
                    continue;
                }                                
                return operand_rvalue(aggregate_item->type);
            }            
        }                   

        return operand_none();
    }
    Operand resolve_field(Expr* parent, Expr* children){        
        assert(children->kind == Ast::EXPR_NAME);        
        Operand field_enum = resolve_field_as_enum(parent, children);
        
        if(!field_enum.isNone()){            
            return field_enum;
        }
                        
        Operand op = resolve_expr(parent);                
        if(Type* struct_base = auto_derref_struct_for_field_access(op.type)){
            op.type = struct_base;
        }
        // Check for TypeImpl in op.type      
        Operand type_impl = resolve_field_type_impl(op, children->name);        
        if(!type_impl.isNone()){
            return type_impl;
        }        
        if(op.type->kind == TYPE_STRUCT or op.type->kind == TYPE_UNION){                                
            for(TypeField* field: op.type->aggregate.items){
                if(field->name != children->name){                    
                    continue;
                }                                
                resolver_log(parent->loc, "FOUND FIELD %s -> %s\n", op.type->repr(), children->name);
                return operand_rvalue(field->type);
            }            
        }                   
                        
        resolver_error(children->loc, "field not found: '%s'in type %s\n", children->name, op.type->repr());                
        exit(1);
    }
    Operand resolve_ternary(Expr* cond, Expr*  if_case, Expr* else_case){
        // NOTE: ternary will never be mutable
        return operand_lvalue(type_int(64, false), {0});
    }
    Operand create_lambda_callee(SVec<ProcParam*> params, TypeSpec* ret, SVec<Stmt*> block, std::string* dst = nullptr){
        for(ProcParam* param: params){
            resolve_typespec(param->type);            
        }
        resolve_typespec(ret);        

        static int lambda_count = 1;
        std::string callee = "__lambda_" + std::to_string(lambda_count++) + "__";
        if(dst) *dst = callee;

        Decl* d = Utils::decl_proc(ret->loc, callee.c_str(), params, ret, block, true);
        Sym* lambda = sym_new(d->name, d);
        assert(lambda);        
        resolve_sym(lambda);                
        allocate_lambda(lambda);
        resolve_sym(lambda);

    
        return ret->kind == Ast::TYPESPEC_POINTER
            ? operand_rvalue(ret->resolvedTy)
            : operand_lvalue(ret->resolvedTy, {0});
    }
    Operand resolve_lambda(Expr*& expr, std::string* dst = nullptr){        
        printf("[ERROR]: LAMBDAS are not implemented due to scope limitations.\n");
        abort();                
        create_lambda_callee(expr->lambda.params, expr->lambda.ret, expr->lambda.block, dst);                
        Sym* declared_lambda = Declared_lambdas.back();        
        SVec<Expr*> args; args.push(Utils::expr_name(expr->loc, declared_lambda->name));

        *expr = *Utils::expr_call( expr->loc,  Utils::expr_name(expr->loc, "asm"), args );
        return operand_lvalue(declared_lambda->type, {0});
    }
    Operand resolve_new(Expr*& e){
        SVec<Expr*>&    args = e->new_expr.args;
        Expr*&          size = e->new_expr.size;
        TypeSpec*&      type = e->new_expr.type;
        resolve_typespec(type);
        type->resolvedTy->ismut = true;
        Operand list_size = resolve_expr(size);

        if(not list_size.type->isInteger()){
            resolver_error(e->loc, "`new[]` operator expects integer in list size, got `new[%s]`.\n", list_size.type->repr());
        }

        if(args.len() > 0){            
            for(Expr*& arg: args){
                resolve_expr(arg);
            }
            if(0){
                resolver_error(e->loc, "[ERROR]: operator `new` with arguments are not implemented yet.\n");        
            }
        }


        TypeSpec* type_allocated = Utils::typespec_pointer(type, type->token);
        resolve_typespec(type_allocated);
        type_allocated->resolvedTy->ismut = true;        
        return operand_rvalue(type_allocated->resolvedTy);        
    }

    Operand resolve_expr(Expr* &expr){        
        if(!expr){
            return operand_lvalue(type_void(), {0});
        }

        switch(expr->kind){
            case EXPR_INT:          return operand_lvalue(type_int(64, true), {.ul = expr->int_lit});
            case EXPR_STRING:       return operand_lvalue(type_string(), {});
            case EXPR_NAME:         return resolve_name(expr->name);
            case EXPR_CALL: {
                if(expr->call.base->kind == EXPR_NAME){
                    if(expr->call.base->name == Core::cstr("typeof")){
                        if(expr->call.args.len() != 1){
                            resolver_error(expr->loc,"[ERROR]: typeof expects one argument.\n");
                            exit(1);
                        }
                        Operand arg = resolve_expr(expr->call.args.at(0));
                        const char* repr = arg.type->repr();                        
                        expr = Utils::expr_string(expr->loc, repr);
                        return operand_lvalue(type_string(), {0});
                    }
                }
                return resolve_call(expr->call.base, expr->call.args);            
            }
            case EXPR_INIT_VAR:     return resolve_var_init(expr->init_var.name, expr->init_var.type, expr->init_var.rhs, true, false);
            case EXPR_UNARY:        return resolve_unary(expr->unary.unary_kind, expr->unary.expr);
            case EXPR_BINARY:   {
                if(expr->binary.binary_kind == TK_lesseq){
                    // T: lhs <= rhs --> not(lhs > rhs);                    
                    Expr* lhs = expr->binary.left;
                    Expr* rhs = expr->binary.right;
                    Expr* base_gt = Utils::expr_binary(expr->loc, TK_greater, lhs, rhs);
                    expr = Utils::expr_unary(expr->loc, TK_not, base_gt);
                    return resolve_expr(expr);                    
                }          
                return resolve_binary(expr->binary.binary_kind, expr->binary.left, expr->binary.right);                
            }            
            case EXPR_ARRAY_INDEX:  return resolve_index(expr);
            case EXPR_CAST:         return resolve_cast(expr->cast.typespec, expr->cast.expr);
            case EXPR_FIELD:        return resolve_field(expr->field_acc.parent, expr->field_acc.children);
            case EXPR_TERNARY:      return resolve_ternary(expr->ternary.cond, expr->ternary.if_case, expr->ternary.else_case);
            case EXPR_LAMBDA:       return resolve_lambda(expr);
            case EXPR_NEW:          return resolve_new(expr);
        
        }
        printf("[ERROR]: Couldn't resolve the following expression:\n");
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
                Procedures::Procedure* current_procedure = ctx.getCurrentProcedure();
                if(!current_procedure){
                    printf("wtf where is CBridge::ctx.Cp?\n");
                    exit(1);                
                }
                Operand got         = resolve_expr(stmt->expr);                
                Type*   expected    = current_procedure->returnType->proc.ret_type;
                if(!expected->typeCheck(got.type)){
                    printf("ERROR AT: "); 
                    pPrint::expr(stmt->expr);
                    printf("\nin cp = %s\n", current_procedure->name);
                    printf("[ERROR]: expected %s in the return of the procedure %s, but got: %s\n",
                        expected->repr(),
                        current_procedure->name,
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
        for(Stmt* stmt: block) {
            resolve_stmt(stmt);
        }        
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
    const char* format_impl_procedure_name(Sym* sym, Decl* node){
        return Core::cstr(strf("%s_impl_%s", sym->name, node->name));
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
        
        ctx.setSelf(sym->type);        
        resolver_log(sym->decl->loc, "Setting self :: %s\n", sym->type->repr());

        
        sym->type->isSelf = true;
        impl_ctx = true;
        for(Decl* node: body){    
            if(node->name == Core::cstr("new")){
                printf("[ERROR]: Can't name a implementation procedure using `new`.\n");
                exit(1);
            }
            
            node->name = format_impl_procedure_name(sym, node);
            
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
        ctx.clearSelf();
        impl_ctx = false;
    }
    void resolve_decl_var(Decl* &d){
        resolve_var_init(d->name, d->var.type, d->var.init, false, false);        
    }
    const char* decorator_flags[] = {
        STRUCT_REASSIGN_FLAG,
        NEW_FLAG
    };
    void show_flags(){
        
        printf("Flags: [ ");
        for(const char*& flag: decorator_flags){                 
            printf("@%s%s", 
                flag, 
                SAD_printf_cond(flag, decorator_flags));        
        }

        printf(" ]");
    }

    void resolve_decl_flags(SVec<Expr*> args, Decl* &d){
        assert(d);
        for(Expr* &arg: args){            
            if(arg->kind != EXPR_STRING){                
                printf("[ERROR]: flag preprocessor expect ONLY string in it's arguments");
                exit(1);
            }
            const char* flag = cstr(arg->string_lit);
            
            if(flag == STRUCT_REASSIGN_FLAG){
                struct_reassigner = d;  
            } else if(flag == NEW_FLAG){
                if(new_allocator){
                    printf("[ERROR]: Multiple configurations of `new` operator in pietra compiler.\n");
                    printf("[INFO]: `new` configured in `%s` but tried to reconfigure in `%s`.\n", new_allocator->name, d->name);
                    exit(1);
                }                
                new_allocator = d;                                
            } else {
                // TODO: put error and info in respective functions
                printf("[ERROR]: the flag `%s` is unkown by the resolver:\n", arg->string_lit);
                printf("[INFO]: Possible flags:\n");
                show_flags();
                exit(1);
            }
        }
    }
    void eval_decorator_method_of(const char* id, Decl *& decl){        
        (void)  id;
        (void)  decl;
        
        printf("[WARN]: Flag method_of is not implemented yet.\n");        
        exit(1);
    }
    void sym_set_used(Sym* sym, bool state){
        if(sym->kind == SYM_PROC){
            Procedure* proc = ASSERT_PTR(ctx.findProcedureByName(sym->name));
            printf("PROPAGATE USED FOR :%s\n", proc->name);
            proc->is_used = state;
        }
    }
    void propagate_used_feature_expr_name(const char* name){
        Sym* sym = sym_get(name);
        assert(sym);
        sym_set_used(sym, true);
    }
    void propagate_used_feature(Expr* expr){
        switch(expr->kind){
            case EXPR_INT:          
            case EXPR_STRING:       
            case EXPR_NAME:         return propagate_used_feature_expr_name(expr->name);
            // TODO: check those nodes
            //case EXPR_CALL: {
            //    if(expr->call.base->kind == EXPR_NAME){
            //        if(expr->call.base->name == Core::cstr("typeof")){
            //            if(expr->call.args.len() != 1){
            //                resolver_error(expr->loc,"[ERROR]: typeof expects one argument.\n");
            //                exit(1);
            //            }
            //            Operand arg = resolve_expr(expr->call.args.at(0));
            //            const char* repr = arg.type->repr();                        
            //            expr = Utils::expr_string(expr->loc, repr);
            //            return operand_lvalue(type_string(), {0});
            //        }
            //    }
            //    return resolve_call(expr->call.base, expr->call.args);            
            //}
            //case EXPR_INIT_VAR:     return resolve_var_init(expr->init_var.name, expr->init_var.type, expr->init_var.rhs, true, false);
            //case EXPR_UNARY:        return resolve_unary(expr->unary.unary_kind, expr->unary.expr);
            //case EXPR_BINARY:   {
            //    if(expr->binary.binary_kind == TK_lesseq){
            //        // T: lhs <= rhs --> not(lhs > rhs);                    
            //        Expr* lhs = expr->binary.left;
            //        Expr* rhs = expr->binary.right;
            //        Expr* base_gt = Utils::expr_binary(expr->loc, TK_greater, lhs, rhs);
            //        expr = Utils::expr_unary(expr->loc, TK_not, base_gt);
            //        return resolve_expr(expr);                    
            //    }          
            //    return resolve_binary(expr->binary.binary_kind, expr->binary.left, expr->binary.right);                
            //}            
            //case EXPR_ARRAY_INDEX:  return resolve_index(expr);
            //case EXPR_CAST:         return resolve_cast(expr->cast.typespec, expr->cast.expr);
            //case EXPR_FIELD:        return resolve_field(expr->field_acc.parent, expr->field_acc.children);
            //case EXPR_TERNARY:      return resolve_ternary(expr->ternary.cond, expr->ternary.if_case, expr->ternary.else_case);
            //case EXPR_LAMBDA:       return resolve_lambda(expr);
            //case EXPR_NEW:          return resolve_new(expr);
        
        }
    }
    void propagate_used_feature_stmt(Stmt* node){
        switch(node->kind){            
            case STMT_EXPR:
                propagate_used_feature(node->expr);
                return;

        }
    }

    void propagate_used_sym(Sym* sym){
            Procedure* proc = ctx.findProcedureByName(sym->name);
            if(proc){
                assert(sym && sym->decl);
                for(Stmt* root_node: sym->decl->proc.block){
                    propagate_used_feature_stmt(root_node);
                }
            }

            else {
                assert(0);
            }


            
    }
    void resolve_decl_proc(Decl* &d, Type* type){                
        // CHANGE: added scopes
        resolver_log(d->loc, "Entering a procedure: %s\n", d->name);
        
        
        if(d->name == Core::cstr("new")){
            printf("[ERROR]: Can't name a procedure with `new` identifier.\n");
            exit(1);
        }
        if(d->proc.is_internal) {            
            Procedure* extern_proc = Factory::createProcedure(d->name, type);  
            extern_proc->is_extern = true;
            ctx.addProcedure(extern_proc);            
            return;
        }
        if(d->notes.len() > 0){
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
                    if(string_comparator){
                        printf("[ERROR]: multiples definition of @string_comparator\n");
                        exit(1);
                    }
                    
                    string_comparator = d;    
                }
                else if(note->name == Core::cstr("flag")){                    
                    resolve_decl_flags(note->args, d);
                }
                else if(note->name == Core::cstr("method_of")){
                    assert(note->args.at(0)->kind == EXPR_STRING);
                    const char* id = note->args.at(0)->string_lit;
                    eval_decorator_method_of(id, d);
                }
                else if(note->name == Core::cstr("extern")){                    
                    Procedure* extern_proc = Factory::createProcedure(d->name, type);  
                    extern_proc->is_extern = true;
                    ctx.addProcedure(extern_proc);
                    return;
                }
                else {
                    show_all_decorators();
                    printf("[ERROR]: compiler doesn't understand the decl note %s\n", note->name);
                    exit(1);
                }
            }
        }
        assert(d->kind == DECL_PROC);        
        Procedures::Procedure* proc = Factory::createProcedure(d->name, type);        
        
        ctx.addProcedure(proc);                
        ctx.getLocalScope()->enter_scope();                
        for(ProcParam* pp: d->proc.params){                        
            resolve_var_init(pp->name, pp->type, pp->init, false, true);
            resolver_log(pp->loc, "Resolved procedure %s argument %s: %s\n", d->name, pp->name, pp->type->resolvedTy->repr());
        }        
        resolve_stmt_block(d->proc.block);
        ctx.getLocalScope()->leave_scope();        
        ctx.currentProcedureRewind();                
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
                if(item->init->kind != EXPR_INT){
                    printf("[ERROR]: expected initializer in enumeration item to be an integer.\n");
                    exit(1);
                }
                count = item->init->int_lit;
            }            
            item->name = Core::cstr(strf("%s_impl_%s", d->name, item->name));
            Sym* item_sym = sym_new(item->name, Utils::decl_constexpr(d->loc, item->name, Utils::expr_int(d->loc, count)));            
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
            case DECL_CONSTEXPR: {                            
                Constants::ConstantExpression* const_expr = Factory::createConstantExpression(decl->name, decl->expr);
                ctx.addConstantExpression(const_expr);                
            } break;                 
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

        sym->state = SYM_RESOLVED;
        if(sym->decl){             
            if(sym->decl->kind == DECL_PROC and sym->decl->proc.params.len() > 0){                
                if(sym->decl->proc.params.at(0)->name == Core::cstr("self")){                    
                    printf("[ERROR]: The parameter named self is reserved to be inside implementation context.\n");
                    exit(1);
                }
                
            }
            
            // some syms are expressions            
            resolve_decl(sym->decl, sym->type);
            resolved_ast.push(sym->decl);                        
        }        
        sym->state = SYM_RESOLVED;
                
        
        return sym;
    }
    

    SVec<Decl*> resolve_package(PPackage* &package){                    
        ctx.scope->enter_scope();
        declare_package(package);                
        
        for(Sym* sym: global_syms){                                        
            resolve_sym(sym);                        
        }                
        return resolved_ast;
    }    

    
}
#endif /*RESOLVE_CPP*/
