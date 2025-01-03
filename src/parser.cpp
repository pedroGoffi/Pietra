#ifndef PIETRA_PARSER
#define PIETRA_PARSER
#include "ast.cpp"
#include "lexer.cpp"
#include "file.cpp"
#include "interns.cpp"
#include "package.cpp"
#include "../include/parser.hpp"
#include <cassert>
#include <iterator>
#include "preprocess.cpp"

using namespace Pietra;
using namespace Pietra::Ast;
#define unimplemented() printf("[ERR]: the %s is unimplemented for: %s", __FUNCTION__, lexer->token.name); exit(1);
//TypeSpec* proc_params(Lexer* lexer);


namespace Pietra::Parser {
PPackage* cpack;    

Expr* literal_expr_assign(Lexer* lexer, const char* name){
    assert(lexer->expect(TK_colon));    
    TypeSpec*   ts   = nullptr;
    Expr*       init = nullptr;
    if(!lexer->token.isKind(TK_eq)){
        ts = typespec(lexer);
    }
    if(lexer->token.isKind(TK_eq)){
        lexer->nextToken();
        init = expr(lexer);
    }    
    return Utils::expr_assign(lexer->getLocation(), name, ts, init);

}
Expr* new_expr(Lexer* lexer){
    //assert(lexer->token.isKind(TK_NEW));
    lexer->nextToken();
    Expr* list_items_number = nullptr;
    if(lexer->expect(TK_lbracket)){        
        list_items_number = expr(lexer);                    
        assert(lexer->expect(TK_rbracket));        
    }
    else {
        list_items_number = Utils::expr_int(lexer->getLocation(), 1);
    }
    TypeSpec* type = typespec(lexer);
    SVec<Expr*> args;
    if(lexer->expect(TK_lparen)) {    
        args = expr_list(lexer);        
        assert(lexer->expect(TK_rparen));        
    }        
    return Utils::expr_new(lexer->getLocation(), type, list_items_number, args);    
}
Expr* literal_expr(Lexer* lexer){    
    if(lexer->token.isKind(TK_name)){
        if(lexer->token.name == switch_keyword){
            Stmt* st_switch = stmt_switch(lexer);
            return Utils::expr_switch(
                lexer->getLocation(), 
                st_switch->stmt_switch.cond, 
                st_switch->stmt_switch.cases,                  
                st_switch->stmt_switch.has_default,
                st_switch->stmt_switch.default_case);                

        } else if(lexer->token.name == new_keyword) return new_expr(lexer);

        const char* name = lexer->token.name;        
        lexer->nextToken();
        if(lexer->token.isKind(TK_colon)){
            // fast assign var :type = expr
            return literal_expr_assign(lexer, name);
        }
        else {
            return Utils::expr_name(lexer->getLocation(), name);
        }
    } else if(lexer->token.isKind(TK_int)){
        uint64_t i64 = lexer->token.i64;
        lexer->nextToken();
        return Utils::expr_int(lexer->getLocation(), i64);    
    } else if(lexer->token.isKind(TK_float)){
        double f64 = lexer->token.f64;
        lexer->nextToken();
        return Utils::expr_float(lexer->getLocation(), f64);
    } else if(lexer->token.isKind(TK_dqstring)){
        const char* str = lexer->token.string;
        lexer->nextToken();
        return Utils::expr_string(lexer->getLocation(), str);
    } else if(lexer->token.isKind(TK_sqstring)){
        const char* str = lexer->token.string;
        lexer->nextToken();        
        char character = *str;
        return Utils::expr_int(lexer->getLocation(), character);

    } else if(lexer->token.isKind(TK_lparen)){
        lexer->nextToken();
        if(lexer->token.isKind(TK_colon)){            
            TypeSpec* ts = typespec(lexer);
            assert(lexer->expect(TK_rparen));
            Expr* base = unary_expr(lexer);
            return Utils::expr_cast(lexer->getLocation(), ts, base);
        }
        else {
            Expr* e = expr(lexer);
            assert(lexer->expect(TK_rparen));        
            return e;
        }

    }
    else {        
        syntax_error(lexer->getLocation(), "expected expression, got: (%s) kind %s\n", 
            lexer->token.name,
            tokenKindRepr(lexer->token.kind));    

        lexer->nextToken();
        return nullptr;
    }
}

inline bool is_base_expr(Lexer* lexer){
    return  lexer->token.isKind(TK_dot)
        |   lexer->token.isKind(TK_lparen)
        |   lexer->token.isKind(TK_lbracket)
        ;    
}
Expr* base_expr(Lexer* lexer){
    Expr* base = literal_expr(lexer);
    while(is_base_expr(lexer)){
        if(lexer->token.isKind(TK_dot)){
            lexer->nextToken();
            Expr* child = literal_expr(lexer);            
            base = Utils::expr_field_access(lexer->getLocation(), base, child);
        }  else if(lexer->token.isKind(TK_lbracket)){
            lexer->nextToken();

            Expr* arr_field = nullptr;
            if(!lexer->token.isKind(TK_rbracket)){
                arr_field = expr(lexer);
            }

            assert(lexer->token.isKind(TK_rbracket));
            lexer->nextToken();

            base = Utils::expr_array_index(lexer->getLocation(), base, arr_field);

        } else if(lexer->token.isKind(TK_lparen)){
            lexer->nextToken();
            SVec<Expr*> args = {};

            if(!lexer->token.isKind(TK_rparen)){
                args = expr_list(lexer);
            }

            if(!lexer->token.isKind(TK_rparen)){
                syntax_error(lexer->getLocation(), "Expected ')' on closing of procedure call: got %s.\n", lexer->token.name);
                lexer->nextToken();
                return nullptr;
            }
            lexer->nextToken();
            base = Utils::expr_call(lexer->getLocation(), base, args);

        }
        else {
            printf("Unimplemented..\n");
            abort();
        }
    }

    return base;
}
static inline bool is_unary_expr(Lexer* lexer){
    return 
            lexer->token.isKind(TK_plus) 
        |   lexer->token.isKind(TK_minus)
        |   lexer->token.isKind(TK_not)
        |   lexer->token.isKind(TK_mul)
        |   lexer->token.isKind(TK_and)
        ;    
}
Expr* unary_expr(Lexer* lexer){        
    while(is_unary_expr(lexer)){
        TokenKind kind = lexer->token.kind;
        lexer->nextToken();                
        return Utils::expr_unary(lexer->getLocation(), kind, unary_expr(lexer));
    }    


    return base_expr(lexer);   
}
static inline bool is_mult_expr(Lexer* lexer){
    return 
            lexer->token.isKind(TK_mul)     or
            lexer->token.isKind(TK_div)     or
            lexer->token.isKind(TK_or)      or
            lexer->token.isKind(TK_mod)
    ;
}
Expr* mult_expr(Lexer* lexer){
    Expr* unary = unary_expr(lexer);
    while(is_mult_expr(lexer)){
        TokenKind kind = lexer->token.kind; 
        lexer->nextToken();
        Expr* rhs = unary_expr(lexer);

        unary = Utils::expr_binary(lexer->getLocation(), kind, unary, rhs);
    }
    return unary;
}
static inline bool is_add_expr(Lexer* lexer){
    return 
            lexer->token.isKind(TK_plus) or
            lexer->token.isKind(TK_minus)
    ;
}

Expr* add_expr(Lexer* lexer){
    Expr* mult = mult_expr(lexer);
    while(is_add_expr(lexer)){
        TokenKind kind = lexer->token.kind;
        lexer->nextToken();
        Expr* rhs = mult_expr(lexer);

        mult = Utils::expr_binary(lexer->getLocation(), kind, mult, rhs);

    }
    return mult;
}
static inline bool is_cmp_expr(Lexer* lexer){
    return  lexer->token.isKind(TK_eqeq)    
    |       lexer->token.isKind(TK_less)    
    |       lexer->token.isKind(TK_greater) 
    |       lexer->token.isKind(TK_lesseq)  
    |       lexer->token.isKind(TK_noteq)
    ;    
}
Expr* cmp_expr(Lexer* lexer){
    Expr* add = add_expr(lexer);
    while(is_cmp_expr(lexer)){
        TokenKind kind = lexer->token.kind;
        lexer->nextToken();
        Expr* rhs = add_expr(lexer);

        add = Utils::expr_binary(lexer->getLocation(), kind, add, rhs);
    }
    return add;
}
static inline bool is_logic_expr(Lexer* lexer){
    return  lexer->matchKeyword(and_keyword)
    |       lexer->matchKeyword(or_keyword)
    |       lexer->token.isKind(TK_oror)
    |       lexer->token.isKind(TK_andand)    
    ;
}
Expr* logic_expr(Lexer* lexer){
    // TODO: wrap this to var
    if(lexer->token.name == cstr("not")){   
        TokenKind kind = TK_not;
        lexer->nextToken();
        return Utils::expr_unary(lexer->getLocation(), kind, logic_expr(lexer));
    }

    Expr* cmp = cmp_expr(lexer);
    while(is_logic_expr(lexer)){
        TokenKind kind = lexer->token.kind;
        if(lexer->token.isKind(TK_name)){
            kind = lexer->matchKeyword(and_keyword) ? TK_andand : TK_oror;
        }
        lexer->nextToken();
        Expr* rhs = cmp_expr(lexer);
        cmp = Utils::expr_binary(lexer->getLocation(), kind, cmp, rhs);
    }
    return cmp;
}

Expr* ternary_expr(Lexer* lexer){
    Expr* logic = logic_expr(lexer);

    if(lexer->token.isKind(TK_question)){
        lexer->nextToken();
        Expr* then = ternary_expr(lexer);
        if(!lexer->token.isKind(TK_colon)){
            printf("[ERROR]: expected ':' while parsing ternary.\n");
            exit(1);
        }
        lexer->nextToken();
        Expr* otherwise = ternary_expr(lexer);
        
        logic = Utils::expr_ternary(lexer->getLocation(), logic, then, otherwise);
    }
    // TODO: implement lexing of ?? operator
    // else if(lexer->token.isKind(TK_DQUESTION)){
    //     lexer->nextToken();        
    //     Expr* rhs = ternary_expr(lexer);
    //     logic = Utils::expr_ternary(logic, logic, rhs);
    // }

    return logic;
}
Expr* cast_expr(Lexer* lexer){        
    Expr* logic = ternary_expr(lexer);
    if(lexer->token.name == cstr("as")){
        lexer->nextToken();
        TypeSpec* ts = typespec(lexer);
        return Utils::expr_cast(lexer->getLocation(), ts, logic);
    }
    return logic;
}


static inline bool is_assign(Lexer* lexer){
    return
            lexer->token.isKind(TK_eq)
    ;
}

Expr* assign_expr(Lexer* lexer){
    Expr* cast = cast_expr(lexer);
    if(is_assign(lexer)){
        TokenKind kind = lexer->token.kind;
        lexer->nextToken();
        Expr* rhs = expr(lexer);
        cast = Utils::expr_binary(lexer->getLocation(), kind, cast, rhs);
    }
    return cast;
}
Expr* expr_lambda(Lexer* lexer){    
    lexer->nextToken();
    SVec<ProcParam*>    params = proc_params(lexer);
    TypeSpec*           ret    = nullptr;
    if(lexer->token.isKind(TK_colon)){
        lexer->nextToken();
        ret = typespec(lexer);
    }

    if(!lexer->token.isKind(TK_lcurly)){
        printf("[ERROR]: lambda expressions expects syntax lambda (...): ret {body}\n");
        printf("Got %s\n", lexer->getPtr());
        exit(1);        
    }

    SVec<Stmt*> block = stmt_opt_curly_block(lexer);

    return Utils::expr_lambda(lexer->getLocation(), params, ret, block);    
}
Expr* expr(Lexer* lexer){
    if(lexer->token.name == cstr("proc")){
        return expr_lambda(lexer);
    }
    return assign_expr(lexer);
}
SVec<Expr*> expr_list(Lexer* lexer){
    SVec<Expr*> list;
    do {
        if(list.len() > 0){
            if(!lexer->token.isKind(TK_comma)){
                syntax_error(lexer->getLocation(), "Expected ',' between expression list, got: %s.\n", lexer->token.name);
                lexer->nextToken();
                return {};
            }
            lexer->nextToken();
        }
        list.push(expr(lexer));
    } while(lexer->token.isKind(TK_comma));
    return list;
}
TypeSpec* typespec_base(Lexer* lexer){
    if(lexer->token.isKind(TK_name)){        
        const char* name = lexer->token.name;
        lexer->nextToken();
        return Utils::typespec_name(name, lexer->token);        
    }
    else {
        resolver_error(lexer->getLocation(), "[ERR]: expected typespec, got: %s\n", lexer->token.name);        
        exit(1);
    }
}
static inline bool is_typespec(Lexer* lexer){
    return 
            lexer->token.isKind(TK_less) // fot templates TYPESPEC<TYPESPEC>
    ;
}
TypeSpec* proc_type(Lexer* lexer){
    assert(lexer->token.name == cstr("proc"));
    lexer->nextToken();
    if(!lexer->token.isKind(TK_lparen)){
        printf("[ERROR]: expected %s token while trying to parse proc type.\n", tokenKindRepr(TK_lparen));
        exit(1);
    }
    lexer->nextToken();
        
    SVec<TypeSpec *> params         = {};
    TypeSpec         *ret           = nullptr;
    bool             has_varags     = false;
    Token           tk              = lexer->token;    
    while(!lexer->token.isKind(TK_rparen)){        
        if(params.len() > 0){
            if(!lexer->token.isKind(TK_comma)){
                printf("[ERROR]: expected ',' while trying to parse the type proc.\n");
                exit(1);
            }
            lexer->nextToken();
        }
        if(lexer->expect(TK_tripledot)){
            has_varags = true;
            TypeSpec* va = Utils::proc_param_varargs()->type;            
            params.push(va);
        }
        else {
            if(has_varags){
                printf("[ERROR]: variad args must be the last argument in procedures.\n");
                exit(1);
            }
            TypeSpec* type = typespec(lexer);
            params.push(type);     
        }
    }    
    if(!lexer->token.isKind(TK_rparen)){
        printf("[ERROR]: expected %s token while trying to parse proc type.\n", tokenKindRepr(TK_rparen));
        exit(1);
    }    
    lexer->nextToken();

    if(lexer->token.isKind(TK_colon)){
        lexer->nextToken();
        ret = typespec(lexer);
    }
    return Utils::typespec_proc(params, ret, has_varags, tk);
}
TypeSpec* typespec(Lexer* lexer){    
    if(lexer->token.isKind(TK_lbracket)){
        lexer->nextToken();
        Expr* arr_size = nullptr;
        if(!lexer->token.isKind(TK_rbracket)){            
            arr_size = expr(lexer);
        } 
        assert(lexer->token.isKind(TK_rbracket));
        lexer->nextToken();

        return Utils::typespec_array(typespec(lexer), arr_size, lexer->token);
    } else if(lexer->token.name == const_keyword){
        lexer->nextToken();
        return Utils::typespec_const(typespec(lexer), lexer->token);
    } else if(lexer->token.isKind(TK_mul)){
        lexer->nextToken();
        return Utils::typespec_pointer(typespec(lexer), lexer->token);
    } else if(lexer->token.name == cstr("proc")){        
        return proc_type(lexer);
    } else if(lexer->token.name == cstr("mut")){
        lexer->nextToken();
        TypeSpec* ts = typespec(lexer);
        ts->mutablity = true;
        return ts;        
    } 
    // NOTE: imut is equivalent to const 
    else if(lexer->token.name == cstr("imut") or lexer->token.name == cstr("mut")){
        lexer->nextToken();
        TypeSpec* ts = typespec(lexer);
        ts->mutablity = false;
        return ts;
    }
    else {
        TypeSpec* ts = typespec_base(lexer);                
        while(is_typespec(lexer)){
            if(lexer->token.kind == TK_less){
                lexer->nextToken();
                TypeSpec* base = typespec(lexer);
                assert(lexer->token.isKind(TK_greater));
                lexer->nextToken();
                ts = Utils::typespec_template(ts, base, lexer->token);
            }
            else {
                unimplemented();
            }
        }
        return ts;
    }    
}

SVec<Stmt*> stmt_opt_curly_block(Lexer* lexer){    
    SVec<Stmt*> block = {};

    if(lexer->token.isKind(TK_lcurly)){
        lexer->nextToken();
        while(!lexer->token.isKind(TK_rcurly)){
            Stmt* st = stmt(lexer);
            if(st){
                block.push(st);            
            }
        }

        assert(lexer->token.isKind(TK_rcurly));
        lexer->nextToken();
    }
    else {
        Stmt* st = stmt(lexer);
        if(st){
            block.push(st);            
        }
    }    
    return block;
}
IfClause* stmt_if_clause(Lexer* lexer){
    Expr* e             = expr(lexer);
    SVec<Stmt*> block   = stmt_opt_curly_block(lexer);
    return Utils::init_if_clause(e, block);
}


static inline bool is_switch_case_pattern(Lexer* lexer){
    return  lexer->token.isKind(TK_name) 
    |       lexer->token.isKind(TK_int) 
    |       lexer->token.isKind(TK_float) 
    |       lexer->token.isKind(TK_dqstring) 
    ;
}
SwitchCasePattern* switch_case_pattern(Lexer* lexer){
    // 1 (..end | , pattern )
    SwitchCasePattern* pattern = Utils::init_pattern();    
    

    if(lexer->token.isKind(TK_int)){
        pattern->begin = lexer->token.i64;
        lexer->nextToken();
    } 
    else if(lexer->token.isKind(TK_name)){
        pattern->begin = pattern->end;
        pattern->name = lexer->token.name;
        lexer->nextToken();
    } else if(lexer->token.isKind(TK_dqstring)){        
        pattern->string = lexer->token.string;
        lexer->nextToken();        
    } else if(lexer->token.isKind(TK_sqstring)) {
        pattern->begin = *lexer->token.string;
        lexer->nextToken();
    }
    else {
        
        syntax_error(lexer->getLocation(), "unknown switch case pattern token kind");
        lexer->nextToken();
        return nullptr;
    }

    if(lexer->token.isKind(TK_tripledot)){
        lexer->nextToken();
        pattern->end = lexer->token.i64;
        lexer->nextToken();
    }    
    return pattern;
}
SVec<SwitchCasePattern*> switch_case_patterns(Lexer* lexer){
    SVec<SwitchCasePattern*> patterns;
    do {
        if(patterns.len() > 0){
            assert(lexer->token.isKind(TK_comma));
            lexer->nextToken();
        }

        patterns.push(switch_case_pattern(lexer));
    } while(lexer->token.isKind(TK_comma));
    
    return patterns;
}

SwitchCase* switch_case(Lexer* lexer){
    assert(lexer->token.name == cstr("case"));
    lexer->nextToken();

    SVec<SwitchCasePattern*> patterns = switch_case_patterns(lexer);    
    assert(lexer->token.isKind(TK_colon));
    lexer->nextToken();
    SVec<Stmt*> block = stmt_opt_curly_block(lexer);

    return Utils::switch_case(patterns, block);
}
static inline bool is_case(Lexer* lexer){    
    return 
        lexer->token.name == cstr("case")
    |   lexer->token.name == cstr("default")
    ;
}
Stmt* stmt_switch(Lexer* lexer){
    assert(lexer->token.name == switch_keyword);
    lexer->nextToken();
    Expr*               cond            = expr(lexer);
    SVec<SwitchCase*>   cases           = {};
    bool                has_default     = false;
    SVec<Stmt*>         default_block   = {};

    assert(lexer->token.isKind(TK_lcurly));
    lexer->nextToken();
    while(is_case(lexer)){
        if(lexer->token.name == cstr("case")){            
            cases.push(switch_case(lexer));
        } else if(lexer->token.name == cstr("default")){
            lexer->nextToken();
            assert(lexer->token.isKind(TK_colon));
            lexer->nextToken();

            has_default = true;
            default_block = stmt_opt_curly_block(lexer);
        }
        else {
            printf("[ERR]: switch case not implemented for keyword: `%s`\n", lexer->token.name);
            exit(1);
        }
    }
    assert(lexer->token.isKind(TK_rcurly));
    lexer->nextToken();



    return Utils::stmt_switch(cond, cases, has_default, default_block);
}
Stmt* stmt_for(Lexer* lexer){    
    Expr* init          = nullptr;
    Expr* cond          = nullptr;
    Expr* inc           = nullptr;
    SVec<Stmt*> block   = {};

    assert(lexer->token.name == for_keyword);
    lexer->nextToken();

    if(lexer->token.isKind(TK_lcurly)){
        // for {}
        block = stmt_opt_curly_block(lexer);
    }
    else {
        init = expr(lexer);
        if(init->kind != Ast::EXPR_INIT_VAR){
            printf("[ERROR]: `for` statement incorrect usage.\n");
            exit(1);
        } 

        if(lexer->token.isKind(TK_colon)){
            // for x:type : list
            lexer->nextToken();
            cond = expr(lexer);            
        }
        else {
            // for x:i64 = 0; x < 10; x = x + 1 {}
            assert(lexer->token.isKind(TK_semicolon));
            lexer->nextToken();
            cond = expr(lexer);        
            assert(lexer->token.isKind(TK_semicolon));
            lexer->nextToken();
            inc = expr(lexer);
            
        }
        block = stmt_opt_curly_block(lexer);
        
    }    
    return Utils::stmt_for(init, cond, inc, block);
}
Stmt* stmt_while(Lexer* lexer){
    assert(lexer->token.name == while_keyword);    
    lexer->nextToken();
    Expr* cond          = expr(lexer);
    SVec<Stmt*> block   = stmt_opt_curly_block(lexer);        
    return Utils::stmt_while(cond, block, false);
}
Stmt* stmt_if(Lexer* lexer){        
    assert(lexer->token.name == if_keyword);
    lexer->nextToken();
    IfClause*       if_clause = stmt_if_clause(lexer);
    SVec<IfClause*> elif_clauses;
    SVec<Stmt*>     else_block;

    while(lexer->token.name == cstr("elif")){        
        lexer->nextToken();
        elif_clauses.push(stmt_if_clause(lexer));
    }

    if(lexer->token.name == cstr("else")){
        lexer->nextToken();        
        else_block = stmt_opt_curly_block(lexer);
    }


    return Utils::stmt_if(if_clause, elif_clauses, else_block);    
}
Stmt* stmt_do_while(Lexer* lexer){
    assert(lexer->token.name == cstr("do"));
    lexer->nextToken();
    SVec<Stmt*> block = stmt_opt_curly_block(lexer);
    assert(lexer->token.name == while_keyword);
    lexer->nextToken();
    Expr* cond = expr(lexer);
    return Utils::stmt_while(cond, block, true);
}
Stmt* stmt(Lexer* lexer){              
    if(lexer->token.isKind(TK_semicolon)){
        lexer->nextToken();
        return nullptr;
    }
    if(lexer->token.name == if_keyword){    
        return stmt_if(lexer);
    } else if(lexer->token.name == while_keyword){        
        return stmt_while(lexer);
    } else if(lexer->token.name == for_keyword){
        return stmt_for(lexer);
    } else if(lexer->token.name == switch_keyword){        
        return stmt_switch(lexer);
    } else if(lexer->token.name == return_keyword){        
        lexer->nextToken();        
        return Utils::stmt_return(expr(lexer));
    } else if(lexer->token.name == cstr("do")){
        return stmt_do_while(lexer);        
    }
    else {
        return Utils::stmt_expr(expr(lexer));        
    }
}
ProcParam* proc_param(Lexer* lexer){    
    if(lexer->token.isKind(TK_tripledot)){
        lexer->nextToken();
        return Utils::proc_param_varargs();
    }
    assert(lexer->token.kind == TK_name);
    const char* name = lexer->token.name;
    lexer->nextToken();

    if(!lexer->token.isKind(TK_colon)){
        syntax_error(lexer->getLocation(), "Expected ':', got \"%s\"\n", lexer->token.name);
        lexer->nextToken();
        return nullptr;
    }
    lexer->nextToken();
    TypeSpec* type = typespec(lexer);
    
    Expr* init = nullptr;
    if(lexer->token.isKind(TK_eq)){        
        lexer->nextToken();
        init = expr(lexer);
    } 

    return Utils::proc_param(lexer->getLocation(), name, type, init);
}
SVec<ProcParam*> proc_params(Lexer* lexer){
    SVec<ProcParam*> params;
    if(!lexer->token.isKind(TK_lparen)){
        syntax_error(lexer->getLocation(), "Expected '{' got '%s'\n", lexer->token.name);        
        lexer->nextToken();
        return {};
    }
    lexer->nextToken();
    while(!lexer->token.isKind(TK_rparen)){
        if(params.len() > 0){
            assert(lexer->token.isKind(TK_comma));
            lexer->nextToken();
        }
        ProcParam* param = proc_param(lexer);
        if(param->isVararg){
            if(!lexer->token.isKind(TK_rparen)){
                printf("[ERR]: variadic argument must be the last argument.\n");
                exit(1);
            }
        }
        params.push(param);

    }
    assert(lexer->token.isKind(TK_rparen));
    lexer->nextToken();
    
    return params;
}
Decl* decl_proc(Lexer* lexer, const char* name){    
    if(lexer->token.name == cstr("proc")) lexer->nextToken();
    bool isVarargs = false;
    SVec<ProcParam*> params = proc_params(lexer);
    
    if(params.len() != 0 and params.back()->isVararg){
        isVarargs = true;
    }
    TypeSpec* ret = nullptr;
    if(lexer->token.isKind(TK_colon)){
        lexer->nextToken();
        ret = typespec(lexer);
    }

    SVec<Stmt*> block;
    bool is_complete = true;    

    if(lexer->token.isKind(TK_semicolon)){        
        lexer->nextToken();        
        is_complete = false;                
    } else {    
        block = stmt_opt_curly_block(lexer);
    }
    return Utils::decl_proc(lexer->getLocation(), name, params, ret, block, is_complete, isVarargs);
}

AggregateItem* aggregate_item(Lexer* lexer){
    // NAME ':' or '::'
    assert(lexer->token.isKind(TK_name));        
    SVec<const char*> names;
    TypeSpec*         type  = nullptr;
    Expr*             init  = nullptr;
    do{
        if(names.len() > 0){
            assert(lexer->token.isKind(TK_comma));
            lexer->nextToken();
        }
        names.push(lexer->token.name);
        lexer->nextToken();
        if(lexer->token.isKind(TK_eq)){
            lexer->nextToken();
            init = expr(lexer);
            return Utils::aggregate_item_field(names, type, init);
        }
        
    } while(lexer->token.isKind(TK_comma));

    assert(lexer->token.isKind(TK_colon));
    lexer->nextToken();
    type = typespec(lexer);
    if(lexer->token.isKind(TK_eq)){
        lexer->nextToken();
        init = expr(lexer);
    }

    return Utils::aggregate_item_field(names, type, init);
}
Decl* decl_aggregate(Lexer* lexer, const char* name, aggregateKind kind){
    assert(lexer->token.isKind(TK_lcurly));
    lexer->nextToken();
    SVec<AggregateItem*> items;
    while(!lexer->token.isKind(TK_rcurly)){
        AggregateItem* item = aggregate_item(lexer);

        items.push(item);
    }
    assert(lexer->token.isKind(TK_rcurly));
    lexer->nextToken();
    
    return Utils::decl_aggregate(lexer->getLocation(), name, kind, items);
}
Decl* decl_struct(Lexer* lexer, const char* name){
    if(lexer->token.name == cstr("struct")) lexer->nextToken();
    assert(lexer->token.isKind(TK_lcurly));
    return decl_aggregate(lexer, name, AGG_STRUCT);
}
Decl* decl_union(Lexer* lexer, const char* name){
    assert(lexer->token.name == cstr("union"));
    lexer->nextToken();
    assert(lexer->token.isKind(TK_lcurly));    
    return decl_aggregate(lexer, name, AGG_UNION);
}
Decl* decl_enum(Lexer* lexer, const char* name){
    assert(lexer->token.name == cstr("enum"));
    lexer->nextToken();
    assert(lexer->token.isKind(TK_lcurly));    
    lexer->nextToken();
    SVec<EnumItem*> items;
    
    while(lexer->token.isKind(TK_name)){                
        Expr* init = nullptr;
        const char* name = lexer->token.name;
        lexer->nextToken();
        
        if(lexer->token.isKind(TK_eq)){
            lexer->nextToken();
            init = expr(lexer);
        }
        items.push(Utils::EnumItem_new(name, init));
     
    }

    assert(lexer->token.isKind(TK_rcurly));    
    lexer->nextToken();

    return Utils::decl_enum(lexer->getLocation(), name, items);

}
Decl* decl_base(Lexer* lexer, const char* name, SVec<Note*> notes){
    Decl* decl;
    if(lexer->token.name == cstr("proc") or lexer->token.isKind(TK_lparen)){
        decl = decl_proc(lexer, name);  
    } else if(lexer->token.name == cstr("struct") or lexer->token.isKind(TK_lcurly)){
        decl = decl_struct(lexer, name);
    } else if(lexer->token.name == cstr("union")){
        decl = decl_union(lexer, name);
    } else if(lexer->token.name == cstr("enum")){
        decl = decl_enum(lexer, name);
    } else {                  
        decl = Utils::decl_constexpr(lexer->getLocation(), name, expr(lexer));
    }    

    assert(decl);
    decl->notes = notes;
    return decl;
}
Decl* decl_type(Lexer* lexer){
    assert(lexer->token.name == cstr("type"));
    lexer->nextToken();
    assert(lexer->token.isKind(TK_name));
    const char* name = lexer->token.name;
    lexer->nextToken();

    assert(lexer->token.isKind(TK_scoperes));
    lexer->nextToken();
    TypeSpec* type = typespec(lexer);
    return Utils::decl_type(lexer->getLocation(), name, type);
}
SVec<const char*> use_module_names(Lexer* lexer){    
    if(!lexer->token.isKind(TK_name)) return {};    
    SVec<const char*> names = {};
    do {
        if(names.len() > 0){
            assert(lexer->token.isKind(TK_scoperes));
            lexer->nextToken();
        }
        const char* name = lexer->token.name;
        lexer->nextToken();
        names.push(name);
    } while(*lexer->getPtr() and lexer->token.isKind(TK_scoperes));

    return names;
}
SVec<const char*> use_names(Lexer* lexer){
    SVec<const char*> names = {};
    if(lexer->token.isKind(TK_lcurly)){
        lexer->nextToken();
        do {
            if(names.len() > 0){
                assert(lexer->token.isKind(TK_comma));
                lexer->nextToken();                
            }

            const char* name = lexer->token.name;
            lexer->nextToken();
            names.push(name);
        } while(*lexer->getPtr() and lexer->token.isKind(TK_comma));        
        assert(lexer->token.isKind(TK_rcurly));
        lexer->nextToken();
    }
    return names;
}

Decl* decl_use(Lexer* lexer){
    assert(lexer->token.name == cstr("use"));
    lexer->nextToken();
    SVec<const char*> _mod_names = use_module_names(lexer);
    SVec<const char*> _use_names; 
    if(lexer->token.isKind(TK_lcurly)){
        _use_names = use_names(lexer);
    }
    bool use_all = false;
    if(lexer->token.isKind(TK_mul)){
        lexer->nextToken();
        use_all = true;
    }

    const char* rename = 0;
    if(lexer->token.name == cstr("as")){
        lexer->nextToken();
        rename = Core::cstr(lexer->token.name);
        lexer->nextToken();
    }

    const char* path = ".";
    for(auto& pnode: _mod_names){
        path = strf("%s/%s", path, pnode);
    }
    path = Core::cstr(path);


    // TODO: Check if the package is already included.
    if(_use_names.len() == 0){
        printf("[WARN]: Cant include all package yet, use_names.len == 0.\n");
        exit(1);
    }    
    SVec<Decl*> module;    
    for(const char* use: _use_names){        
        const char* new_path = strf("%s/%s.pi", path, use);
        // TODO: test in here
        //stream_snaphot();
        PPackage* package = PPackage::from(new_path);
        if(!package){
            printf("[ERROR]: Could not find the package in '%s'\n", new_path);
            exit(1);
        }
        for(auto& node: package->ast){
            module.push(node);
        }                
        //stream_rewind();                            
    }
    return Utils::decl_use(lexer->getLocation(), module, use_all, rename);
}

SVec<Expr*> note_args(Lexer* lexer){
    if(!lexer->token.isKind(TK_lparen)){
        return {};        
    }
    lexer->nextToken();    
    SVec<Expr*> args;
    while(*lexer->getPtr() and !lexer->token.isKind(TK_rparen)){
        Expr* arg = expr(lexer);
        assert(arg);
        args.push(arg);
    }
    assert(lexer->token.isKind(TK_rparen));
    lexer->nextToken();
    return args;
}
SVec<Note*> parse_notes(Lexer* lexer){
    SVec<Note*> notes;    
    while(lexer->token.isKind(TK_decorator)){
        lexer->nextToken();
        assert(lexer->token.isKind(TK_name));
        const char* name = lexer->token.name;
        lexer->nextToken();
        SVec<Expr*> args = note_args(lexer);
        Note* note = Utils::init_note(name, args);
        assert(note);
        notes.push(note);        
    }

    return notes;
}
SVec<Decl*> parse_impl_body(Lexer* lexer){
    SVec<Decl*> body;
    if(!lexer->token.isKind(TK_lcurly)){
        Decl* node = decl(lexer);
        if(!node){
            exit(1);
        }
        body.push(node);
    }
    else {
        assert(lexer->token.isKind(TK_lcurly));
        lexer->nextToken();
        while(!lexer->token.isKind(TK_rcurly)){
            Decl* node = decl(lexer);
            if(!node){
                exit(1);
            }
            if(node->kind == DECL_IMPL){
                printf("[ERROR]: Impl can't be nested.\n");
                exit(1);
            }        
            body.push(node);
        }
        assert(lexer->token.isKind(TK_rcurly));
        lexer->nextToken();
    }        
    return body;
} 
Decl* decl_impl(Lexer* lexer){
    assert(lexer->token.name == cstr("impl"));
    lexer->nextToken();
    if(!lexer->token.isKind(TK_name)){
        printf("[ERR]: TODO DOC THIS.\n");
        exit(1);
    }
    const char* target = lexer->token.name;
    lexer->nextToken();
    SVec<Decl*> body = parse_impl_body(lexer);
    
    return Utils::decl_impl(lexer->getLocation(), target, body);
}
Decl* parse_comptime(Lexer* lexer){    
    if(!lexer->token.isKind(TK_name)){
        printf("[ERROR]: comptime expects token name.\n");
        exit(1);
    }

    if(lexer->token.name == cstr("package")){       
        lexer->nextToken();
        if(!lexer->token.isKind(TK_dqstring)){
            printf("[ERROR]: comptime 'package' expects string. #package \"myPackage\".\n");
            exit(1);
        }

        const char* package_name = lexer->token.name;
        lexer->nextToken();
        
        if(is_included(package_name)){                        
            while(lexer->getPtr() and *lexer->getPtr() and *lexer->getPtr()++);            
            lexer->token.kind = TK_eof;            
            return nullptr;
        }        
        // TODO: make the package be in an PPackage list
        include_me(package_name); // Push the package in the packages        
        return nullptr;
    }
    else if(lexer->token.name == cstr("run")){        
        lexer->nextToken();
        if(lexer->token.isKind(TK_dqstring)){
            // Run file 
            const char* file_path = lexer->token.string;
            lexer->nextToken();
            // build the file ast 
            //SVec<Decl*> parser_loop(Lexer* lexer){
            Lexer* preprocess_file_lexer = Lexer::open_at_file(file_path);
            SVec<Decl*> preprocess_svec_ast = parser_loop(preprocess_file_lexer);
            delete preprocess_file_lexer;

            std::vector<Decl*> preprocess_ast;
            for(Decl* node: preprocess_svec_ast){
                preprocess_ast.push_back(node);
         
            }    
            Result file_result = prep_run_ast(preprocess_ast);            
            printf("PREPROCESS FILE RESULT: %s\n", file_result.to_string().c_str());
        }
        else {
            // run block 
            SVec<Stmt*> block = stmt_opt_curly_block(lexer);
            Result block_result = prep_block(block, theContext);
            printf("PREPROCESS BLOCK %s\n", block_result.to_string().c_str());        
        }
        return nullptr;
    }
    else {
        printf("[ERROR]: Undefined comptime expressions '%s'\n", lexer->token.name);
        exit(1);
    }    
}

Decl* decl(Lexer* lexer){
    if(lexer->token.kind == TK_eof or not *lexer->getPtr()) {
        lexer->token.kind = TK_eof; // TO avoid end of the lexer->getPtr() infinite loop
        return nullptr;
    }

    if(lexer->token.kind == TK_hash){
        lexer->nextToken();
        return parse_comptime(lexer);
    }
    
    SVec<Note*> notes = parse_notes(lexer);    
    if(lexer->token.name == cstr("type")){        
        // type cstr :: *char
        return decl_type(lexer);
    } else if(lexer->token.name == cstr("use")){
        return Parser::decl_use(lexer);
        
    } else if(lexer->token.name == cstr("impl")){
        return decl_impl(lexer);
    } 
            
    if( not *lexer->getPtr() ){
        return nullptr;
    }

    const char* name = lexer->token.name;
    lexer->nextToken();
    
    if(lexer->token.isKind(TK_scoperes)){
        lexer->nextToken();
        return decl_base(lexer, name, notes);        
    } else if(lexer->token.isKind(TK_colon)){
        lexer->nextToken();
        TypeSpec* type = Utils::typespec_name("any", lexer->token);
        if(!lexer->token.isKind(TK_eq)) type = typespec(lexer);
        Expr* init = nullptr;
        if(lexer->token.isKind(TK_eq)){
            lexer->nextToken();
            init = expr(lexer);
        }
        return Utils::decl_var(lexer->getLocation(), name, type, init);
    }
    else {
        resolver_error(lexer->getLocation(), "[ERR]: expected declaration, but got: %s\n", lexer->token.name);
        exit(1);
    }
    exit(1);
}

SVec<Decl*> parser_loop(Lexer* lexer){
    SVec<Decl*> ast;
    while(lexer->token.kind != TK_eof){        
        Decl* node = decl(lexer);               
        if(node){        
            ast.push(node);
        } 
        
    }
    return ast;
}
}
#endif /*PIETRA_PARSER*/