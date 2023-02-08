#ifndef PIETRA_PARSER
#define PIETRA_PARSER
#include "ast.cpp"
#include "lexer.cpp"
#include "file.cpp"
#include "interns.cpp"
#include "package.cpp"

#include "../include/parser.hpp"
#include <cassert>
using namespace Pietra;
using namespace Pietra::Ast;
using namespace Pietra::Lexer;
#define unimplemented() printf("[ERR]: the %s is unimplemented for: %s", __FUNCTION__, token.name); exit(1);


namespace Pietra::Parser {
Expr* literal_expr_assign(const char* name){
    assert(is_kind(TK_DDOT));
    next();
    TypeSpec*   ts   = nullptr;
    Expr*       init = nullptr;
    if(!is_kind(TK_EQ)){
        ts = typespec();
    }
    if(is_kind(TK_EQ)){
        next();
        init = expr();
    }    
    return Utils::expr_assign(name, ts, init);

}
Expr* literal_expr(){
    if(is_kind(TK_NAME)){
        const char* name = token.name;
        next();
        if(is_kind(Lexer::TK_DDOT)){
            // fast assign var :type = expr
            return literal_expr_assign(name);
        }
        else {
            return Utils::expr_name(name);
        }
    } else if(is_kind(TK_INT)){
        uint64_t i64 = token.i64;
        next();
        return Utils::expr_int(i64);    
    } else if(is_kind(TK_FLOAT)){
        double f64 = token.f64;
        next();
        return Utils::expr_float(f64);
    } else if(is_kind(TK_STRING)){
        const char* str = token.name;
        next();
        return Utils::expr_string(str);
    } else if(is_kind(Lexer::TK_OPEN_ROUND_BRACES)){
        next();
        if(is_kind(TK_DDOT)){
            // Casting
            next();
            TypeSpec* ts = typespec();
            assert(is_kind(Lexer::TK_CLOSE_ROUND_BRACES));
            next();
            Expr* base = unary_expr();
            return Utils::expr_cast(ts, base);
        }
        else {
            Expr* e = expr();

            assert(is_kind(Lexer::TK_CLOSE_ROUND_BRACES));
            next();
            return e;
        }

    }
    else {
        printf("[ERROR]: expected expression, got: %s\n", token.name);
        next();
        return nullptr;
    }
}

inline bool is_base_expr(){
    return 
            token.kind == Pietra::Lexer::TK_PREP
        |   token.kind == Pietra::Lexer::TK_DOT
        |   token.kind == Pietra::Lexer::TK_OPEN_ROUND_BRACES
        |   token.kind == Pietra::Lexer::TK_OPEN_SQUARED_BRACES        
        ;
}
Expr* base_expr(){
    Expr* base = literal_expr();
    while(is_base_expr()){
        if(is_kind(TK_DOT)){
            next();
            Expr* child = literal_expr();
            
            base = Utils::expr_field_access(base, child);
        }  else if(is_kind(Lexer::TK_OPEN_SQUARED_BRACES)){
            next();

            Expr* arr_field = nullptr;
            if(!is_kind(Lexer::TK_CLOSE_SQUARED_BRACES)){
                arr_field = expr();
            }

            assert(is_kind(Lexer::TK_CLOSE_SQUARED_BRACES));
            next();

            base = Utils::expr_array_index(base, arr_field);

        } else if(is_kind(Lexer::TK_OPEN_ROUND_BRACES)){
            next();
            SVec<Expr*> args = {};

            if(!is_kind(Lexer::TK_CLOSE_ROUND_BRACES)){
                args = expr_list();
            }

            assert(is_kind(Lexer::TK_CLOSE_ROUND_BRACES));
            next();
            base = Utils::expr_call(base, args);

        }
        else {
            unimplemented();
        }
    }

    return base;
}
static inline bool is_unary_expr(){
    return 
            token.kind == Pietra::Lexer::TK_ADD 
        |   token.kind == TK_SUB
        |   token.kind == TK_NOT
        |   token.kind == TK_MULT
        |   token.kind == TK_AMPERSAND
        ;
}
Expr* unary_expr(){        
    while(is_unary_expr()){
        tokenKind kind = token.kind;
        next();                
        return Utils::expr_unary(kind, unary_expr());
    }    


    return base_expr();
    
}
static inline bool is_mult_expr(){
    return 
            token.kind == TK_MULT   or
            token.kind == TK_DIV    or
            token.kind == TK_PIPE   or
            token.kind == TK_MOD
    ;
}
Expr* mult_expr(){
    Expr* unary = unary_expr();
    while(is_mult_expr()){
        tokenKind kind = token.kind; 
        next();
        Expr* rhs = unary_expr();

        unary = Utils::expr_binary(kind, unary, rhs);
    }
    return unary;
}
static inline bool is_add_expr(){
    return 
            token.kind == TK_ADD
        |   token.kind == TK_SUB
    ;
}
Expr* add_expr(){
    Expr* mult = mult_expr();
    while(is_add_expr()){
        tokenKind kind = token.kind;
        next();
        Expr* rhs = mult_expr();

        mult = Utils::expr_binary(kind, mult, rhs);

    }
    return mult;
}
static inline bool is_cmp_expr(){
    return 
            token.kind == TK_CMPEQ
        |   token.kind == TK_LT
        |   token.kind == TK_GT
        |   token.kind == TK_LTE
        |   token.kind == TK_NEQ

    ;
}
Expr* cmp_expr(){
    Expr* add = add_expr();
    while(is_cmp_expr()){
        tokenKind kind = token.kind;
        next();
        Expr* rhs = add_expr();

        add = Utils::expr_binary(kind, add, rhs);
    }
    return add;
}
static inline bool is_logic_expr(){
    return 
            token.name == keyword_land
        |   token.name == keyword_lor
    ;
}
Expr* logic_expr(){
    
    if(token.name == keyword_not){
        tokenKind kind = TK_NOT;
        

        next();
        return Utils::expr_unary(kind, logic_expr());
    }

    Expr* cmp = cmp_expr();
    while(is_logic_expr()){
        tokenKind kind = token.kind;
        next();
        Expr* rhs = cmp_expr();
        cmp = Utils::expr_binary(kind, cmp, rhs);
    }
    return cmp;
}
Expr* cast_expr(){    
    Expr* logic = logic_expr();
    if(token.name == keyword_as){
        next();
        TypeSpec* ts = typespec();
        return Utils::expr_cast(ts, logic);
    }
    return logic;
}
static inline bool is_assign(){
    return
            token.kind == TK_EQ 
    ;
}
Expr* assign_expr(){
    Expr* cast = cast_expr();
    if(is_assign()){
        tokenKind kind = token.kind;
        next();
        Expr* rhs = expr();
        cast = Utils::expr_binary(kind, cast, rhs);
    }
    return cast;
}
static inline bool is_ternary(){
    return token.name == keyword_if;
}
Expr* ternary_expr(){        
    Expr* cast = assign_expr();    
    if (is_ternary()){
        printf("[WARN]: Ternary expressions are not implemented yet.\n");
        exit(1);
    }
    return cast;
}
Expr* expr(){
    Expr* e = ternary_expr();    
    return e;
}
SVec<Expr*> expr_list(){
    SVec<Expr*> list;
    do {
        if(list.len() > 0){
            assert(is_kind(Lexer::TK_COMMA));
            next();
        }
        list.push(expr());
    } while(is_kind(Lexer::TK_COMMA));
    return list;
}
TypeSpec* typespec_base(){
    if(is_kind(TK_NAME)){
        const char* name = token.name;
        next();
        return Utils::typespec_name(name);
    }
    else {
        printf("[ERR]: expected typespec, got: %s\n", token.name);

        exit(1);
    }
}
static inline bool is_typespec(){
    return 
            token.kind == TK_LT // fot templates TYPESPEC<TYPESPEC>
    ;
}
TypeSpec* typespec(){    
    if(is_kind(Lexer::TK_OPEN_SQUARED_BRACES)){
        next();
        Expr* arr_size = nullptr;
        if(!is_kind(Lexer::TK_CLOSE_SQUARED_BRACES)){            
            arr_size = expr();
        } 
        assert(is_kind(Lexer::TK_CLOSE_SQUARED_BRACES));
        next();

        return Utils::typespec_array(typespec(), arr_size);
    } else if(token.name == keyword_const){
        next();
        return Utils::typespec_const(typespec());
    } else if(token.kind == TK_MULT){
        next();
        return Utils::typespec_pointer(typespec());
    }
    else {
        TypeSpec* ts = typespec_base();
        while(is_typespec()){
            if(token.kind == TK_LT){
                next();
                TypeSpec* base = typespec();
                assert(is_kind(TK_GT));
                next();
                ts = Utils::typespec_template(ts, base);
            }
            else {
                unimplemented();
            }
        }
        return ts;
    }    
}

SVec<Stmt*> stmt_opt_curly_block(){
    SVec<Stmt*> block = {};

    if(is_kind(Lexer::TK_OPEN_CURLY_BRACES)){
        next();
        while(!is_kind(Lexer::TK_CLOSE_CURLY_BRACES)){
            Stmt* st = stmt();
            if(st){
                block.push(st);            
            }
        }

        assert(is_kind(Lexer::TK_CLOSE_CURLY_BRACES));
        next();
    }
    else {
        Stmt* st = stmt();
        if(st){
            block.push(st);            
        }
    }
    return block;
}
IfClause* stmt_if_clause(){
    Expr* e = expr();
    SVec<Stmt*> block = stmt_opt_curly_block();
    return Utils::init_if_clause(e, block);
}


static inline bool is_switch_case_pattern(){
    return 
            token.kind == Pietra::Lexer::TK_NAME
        |   token.kind == Pietra::Lexer::TK_INT
        |   token.kind == Pietra::Lexer::TK_FLOAT    
    ;

}
SwitchCasePattern* switch_case_pattern(){
    // 1 (..end | , pattern )
    SwitchCasePattern* pattern = Utils::init_pattern();
    printf("got = %s\n", token.name);
    pattern->name = 0;
    if(is_kind(TK_INT)){
        pattern->begin = token.i64;
        next();
    } 
    else if(is_kind(TK_NAME)){
        pattern->begin = pattern->end;
        pattern->name = token.name;
        next();
    } else {
        assert(0);
    }

    if(is_kind(Pietra::Lexer::TK_TRIPLE_DOT)){
        next();
        pattern->end = token.i64;
        next();
    }    
    return pattern;
}
SVec<SwitchCasePattern*> switch_case_patterns(){
    SVec<SwitchCasePattern*> patterns;
    do {
        if(patterns.len() > 0){
            assert(is_kind(TK_COMMA));
            next();
        }

        patterns.push(switch_case_pattern());
    } while(is_kind(TK_COMMA));
    
    return patterns;
}

SwitchCase* switch_case(){
    assert(token.name == keyword_case);
    next();

    SVec<SwitchCasePattern*> patterns = switch_case_patterns();    
    assert(is_kind(Lexer::TK_DDOT));
    next();
    SVec<Stmt*> block = stmt_opt_curly_block();

    return Utils::switch_case(patterns, block);
}
static inline bool is_case(){    
    return 
        token.name == keyword_case
    |   token.name == keyword_default
    ;
}
Stmt* stmt_switch(){    
    assert(token.name == keyword_switch);
    next();
    Expr*               cond            = expr();
    SVec<SwitchCase*>   cases           = {};
    bool                has_default     = false;
    SVec<Stmt*>         default_block   = {};

    assert(is_kind(Lexer::TK_OPEN_CURLY_BRACES));
    next();
    while(is_case()){
        if(token.name == keyword_case){            
            cases.push(switch_case());
        } else if(token.name == keyword_default){
            next();
            assert(is_kind(Lexer::TK_DDOT));
            next();

            has_default = true;
            default_block = stmt_opt_curly_block();            
        }
        else {
            printf("[ERR]: switch case not implemented for keyword: `%s`\n", token.name);
            exit(1);
        }
    }
    assert(is_kind(Lexer::TK_CLOSE_CURLY_BRACES));
    next();



    return Utils::stmt_switch(cond, cases, has_default, default_block);
}
Stmt* stmt_for(){    
    Expr* init          = nullptr;
    Expr* cond          = nullptr;
    Expr* inc           = nullptr;
    SVec<Stmt*> block   = {};

    assert(token.name == keyword_for);
    next();

    if(is_kind(Lexer::TK_OPEN_CURLY_BRACES)){
        // for {}
        block = stmt_opt_curly_block();
    }
    else {
        init = expr();
        if(init->kind != Ast::EXPR_INIT_VAR){
            printf("[ERROR]: `for` statement incorrect usage.\n");
            exit(1);
        } 

        if(is_kind(Lexer::TK_DDOT)){
            // for x:type : list
            next();
            cond = expr();            
        }
        else {
            // for x:i64 = 0; x < 10; x = x + 1 {}
            assert(is_kind(Lexer::TK_DCOMMA));
            next();
            cond = expr();        
            assert(is_kind(Lexer::TK_DCOMMA));
            next();
            inc = expr();
            
        }
        block = stmt_opt_curly_block();
        
    }    
    return Utils::stmt_for(init, cond, inc, block);
}
Stmt* stmt_while(){
    assert(token.name == keyword_while);
    next();
    Expr* cond          = expr();
    SVec<Stmt*> block   = stmt_opt_curly_block();
    
    return Utils::stmt_while(cond, block);
}
Stmt* stmt_if(){    
    assert(token.name == keyword_if);
    next();
    IfClause*       if_clause = stmt_if_clause();
    SVec<IfClause*> elif_clauses;
    SVec<Stmt*>     else_block;

    while(token.name == keyword_elif){        
        next();
        elif_clauses.push(stmt_if_clause());
    }

    if(token.name == keyword_else){
        next();        
        else_block = stmt_opt_curly_block();
    }


    return Utils::stmt_if(if_clause, elif_clauses, else_block);    
}
Stmt* stmt(){            
    if(is_kind(Lexer::TK_DCOMMA)){
        next();
        return nullptr;
    }
    if(token.name == keyword_if){
        return stmt_if();
    } else if(token.name == keyword_while){
        return stmt_while();
    } else if(token.name == keyword_for){
        return stmt_for();
    } else if(token.name == keyword_switch){        
        return stmt_switch();
    } else if(token.name == keyword_return){        
        next();        
        return Utils::stmt_return(expr());
    }
    else {
        return Utils::stmt_expr(expr());        
    }
}
ProcParam* proc_param(){    
    if(is_kind(Lexer::TK_TRIPLE_DOT)){
        next();
        return Utils::proc_param_varargs();
    }
    assert(token.kind == TK_NAME);
    const char* name = token.name;
    next();

    assert(is_kind(Lexer::TK_DDOT));
    next();
    TypeSpec* type = typespec();
    
    Expr* init = nullptr;
    if(is_kind(Lexer::TK_EQ)){        
        next();
        init = expr();
    } 

    return Utils::proc_param(name, type, init);
}
SVec<ProcParam*> proc_params(){
    SVec<ProcParam*> params;
    assert(is_kind(Lexer::TK_OPEN_ROUND_BRACES));
    next();
    while(!is_kind(Lexer::TK_CLOSE_ROUND_BRACES)){
        if(params.len() > 0){
            assert(is_kind(Lexer::TK_COMMA));
            next();
        }
        ProcParam* param = proc_param();
        if(param->isVararg){
            if(!is_kind(Lexer::TK_CLOSE_ROUND_BRACES)){
                printf("[ERR]: variadic argument must be the last argument.\n");
                exit(1);
            }
        }
        params.push(param);

    }
    assert(is_kind(Lexer::TK_CLOSE_ROUND_BRACES));
    next();
    
    return params;
}
Decl* decl_proc(const char* name){    
    if(token.name == keyword_proc) next();
    bool isVarargs = false;
    SVec<ProcParam*> params = proc_params();
    
    if(params.len() != 0 and params.back()->isVararg){
        isVarargs = true;
    }
    TypeSpec* ret = nullptr;
    if(is_kind(TK_DDOT)){
        next();
        ret = typespec();
    }

    SVec<Stmt*> block;
    bool is_complete = true;    

    if(is_kind(Lexer::TK_DCOMMA)){        
        next();        
        is_complete = false;                
    } else {    
        block = stmt_opt_curly_block();
    }
    return Utils::decl_proc(name, params, ret, block, is_complete, isVarargs);
}

AggregateItem* aggregate_item(){
    // NAME ':' or '::'
    assert(is_kind(TK_NAME));        
    SVec<const char*> names;
    TypeSpec*         type  = nullptr;
    Expr*             init  = nullptr;
    do{
        if(names.len() > 0){
            assert(is_kind(TK_COMMA));
            next();
        }
        names.push(token.name);
        next();
        if(is_kind(TK_EQ)){
            next();
            init = expr();
            return Utils::aggregate_item_field(names, type, init);
        }
        
    } while(is_kind(TK_COMMA));

    assert(is_kind(TK_DDOT));
    next();
    type = typespec();
    if(is_kind(TK_EQ)){
        next();
        init = expr();
    }

    return Utils::aggregate_item_field(names, type, init);
}
Decl* decl_aggregate(const char* name, aggregateKind kind){
    assert(is_kind(Pietra::Lexer::TK_OPEN_CURLY_BRACES));
    next();
    SVec<AggregateItem*> items;
    while(!is_kind(Pietra::Lexer::TK_CLOSE_CURLY_BRACES)){
        AggregateItem* item = aggregate_item();

        items.push(item);
    }
    assert(is_kind(Pietra::Lexer::TK_CLOSE_CURLY_BRACES));
    next();
    
    return Utils::decl_aggregate(name, kind, items);
}
Decl* decl_struct(const char* name){
    if(token.name == keyword_struct) next();
    assert(is_kind(Pietra::Lexer::TK_OPEN_CURLY_BRACES));
    return decl_aggregate(name, AGG_STRUCT);
}
Decl* decl_union(const char* name){
    assert(token.name == keyword_union);
    next();
    assert(is_kind(Pietra::Lexer::TK_OPEN_CURLY_BRACES));    
    return decl_aggregate(name, AGG_UNION);
}
Decl* decl_enum(const char* name){
    assert(token.name == keyword_enum);
    next();
    assert(is_kind(Pietra::Lexer::TK_OPEN_CURLY_BRACES));    
    next();
    SVec<EnumItem*> items;
    
    while(is_kind(TK_NAME)){                
        Expr* init = nullptr;
        const char* name = token.name;
        next();
        
        if(is_kind(TK_EQ)){
            next();
            init = expr();
        }
        items.push(Utils::EnumItem_new(name, init));
     
    }

    assert(is_kind(Pietra::Lexer::TK_CLOSE_CURLY_BRACES));    
    next();

    return Utils::decl_enum(name, items);

}
Decl* decl_base(const char* name, SVec<Note*> notes){
    Decl* decl;
    if(token.name == keyword_proc or is_kind(Lexer::TK_OPEN_ROUND_BRACES)){
        decl = decl_proc(name);  
    } else if(token.name == keyword_struct or is_kind(Lexer::TK_OPEN_CURLY_BRACES)){
        decl = decl_struct(name);
    } else if(token.name == keyword_union){
        decl = decl_union(name);
    } else if(token.name == keyword_enum){
        decl = decl_enum(name);
    } else {                  
        decl = Utils::decl_constexpr(name, expr());
    }    

    assert(decl);
    decl->notes = notes;
    return decl;
}
Decl* decl_type(){
    assert(token.name == keyword_type);
    next();
    assert(is_kind(TK_NAME));
    const char* name = token.name;
    next();

    assert(is_kind(TK_PREP));
    next();
    TypeSpec* type = typespec();
    return Utils::decl_type(name, type);
}
SVec<const char*> use_module_names(){    
    if(!is_kind(TK_NAME)) return {};    
    SVec<const char*> names = {};
    do {
        if(names.len() > 0){
            assert(is_kind(TK_PREP));
            next();
        }
        const char* name = token.name;
        next();
        names.push(name);
    } while(*stream and is_kind(TK_PREP));

    return names;
}
SVec<const char*> use_names(){
    SVec<const char*> names = {};
    if(is_kind(TK_OPEN_CURLY_BRACES)){
        next();
        do {
            if(names.len() > 0){
                assert(is_kind(TK_COMMA));
                next();                
            }

            const char* name = token.name;
            next();
            names.push(name);
        } while(*stream and is_kind(TK_COMMA));        
        assert(is_kind(TK_CLOSE_CURLY_BRACES));
        next();
    }
    return names;
}
Decl* decl_use(){
    assert(token.name == keyword_use);
    next();
    SVec<const char*> _mod_names = use_module_names();
    SVec<const char*> _use_names; 
    if(is_kind(TK_OPEN_CURLY_BRACES)){
        _use_names = use_names();
    }
    bool use_all = false;
    if(is_kind(TK_MULT)){
        next();
        use_all = true;
    }

    const char* rename = 0;
    if(token.name == keyword_as){
        next();
        rename = token.name;
        next();
    }
    printf("<use> not implemented yet.\n");
    exit(1);
}

SVec<Note*> parse_notes(){
    SVec<Note*> notes;

    while(is_kind(TK_NOTE)){
        next();
        assert(is_kind(TK_NAME));
        const char* name = token.name;
        next();
        SVec<Expr*> args;
        Note* note = Utils::init_note(name, args);
        assert(note);
        notes.push(note);
    }

    return notes;
}
SVec<Decl*> parse_impl_body(){
    SVec<Decl*> body;
    assert(is_kind(Pietra::Lexer::TK_OPEN_CURLY_BRACES));
    next();
    while(!is_kind(Pietra::Lexer::TK_CLOSE_CURLY_BRACES)){
        Decl* node = decl();
        if(!node){
            exit(1);
        }
        if(node->kind == DECL_IMPL){
            printf("[ERROR]: Impl can't be nested.\n");
            exit(1);
        }        
        body.push(node);
    }
    assert(is_kind(Pietra::Lexer::TK_CLOSE_CURLY_BRACES));
    next();
    
    return body;
} 
Decl* decl_impl(){
    assert(token.name == keyword_impl);
    next();
    if(!is_kind(TK_NAME)){
        printf("[ERR]: TODO DOC THIS.\n");
        exit(1);
    }
    const char* target = token.name;
    next();
    SVec<Decl*> body = parse_impl_body();
    
    return Utils::decl_impl(target, body);
}
Decl* decl(){
    if(token.kind == TK_EOF) return nullptr;
    SVec<Note*> notes = parse_notes();

    if(token.name == keyword_type){        
        // type cstr :: *char
        return decl_type();
    } else if(token.name == keyword_use){
        return decl_use();
    } else if(token.name == keyword_impl){
        return decl_impl();
    }
    
    assert(is_kind(TK_NAME));
    const char* name = token.name;
    next();
    
    if(is_kind(Lexer::TK_PREP)){
        next();
        return decl_base(name, notes);        
    } else if(is_kind(Lexer::TK_DDOT)){
        next();
        TypeSpec* type = typespec();
        Expr* init = nullptr;
        if(is_kind(TK_EQ)){
            next();
            init = expr();
        }
        return Utils::decl_var(name, type, init);
    }
    else {
        printf("[ERR]: expected declaration, but got: %s\n", token.name);
        exit(1);
    }
    exit(1);
}
}
#endif /*PIETRA_PARSER*/