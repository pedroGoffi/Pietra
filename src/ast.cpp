#ifndef AST_CPP
#define AST_CPP
#include "../include/smallVec.hpp"
#include "../include/ast.hpp"
#include "../include/arena.hpp"
#include "../include/interns.hpp"
#include "../include/type.hpp"
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstdio>

// TODO: arena allocation

namespace Pietra::Utils {
using namespace Pietra::Ast;

Note* init_note(const char* name, SVec<Expr*> args){
    Note* note = arena_alloc<Note>();
    note->name = name;
    note->args = args;
    return note;
}
Expr* init_expr(ExprKind kind){        
    Expr* e = arena_alloc<Expr>();
    e->kind = kind;
    return e;
}
Expr* expr_int(int64_t i64){
    Expr* e = init_expr(Ast::EXPR_INT);
    e->int_lit = i64;
    return e;
}
Expr* expr_float(double f64){
    Expr* e = init_expr(Ast::EXPR_FLOAT);
    e->float_lit = f64;
    return e;
}
Expr* expr_string(const char* str){
    Expr* e = init_expr(Ast::EXPR_STRING);
    e->string_lit = str;
    return e;
}
Expr* expr_name(const char* name){
    Expr* e = init_expr(Ast::EXPR_NAME);
    e->name = Core::cstr(name);
    return e;
}
Expr* expr_compound(TypeSpec* typespec, SVec<Expr*> expr_list){
    Expr* e = init_expr(Ast::EXPR_COMPOUND);
    //e->compound.typespec = typespec;
    //e->compound.expr_list = expr_list;
    return e;
}
Expr* expr_field_access(Expr* parent, Expr* children){
    Expr* e = init_expr(Ast::EXPR_FIELD);
    e->field_acc.parent = parent;
    e->field_acc.children = children;
    return e;
}
Expr* expr_compound_list_indexing(Expr* base_expr, Expr* rhs){
    Expr* e = init_expr(Ast::EXPR_COMPOUND_LIST_INDEXING);
    e->list_indexing.base_expr = base_expr;
    e->list_indexing.rhs = rhs;
    return e;
}
Expr* expr_compound_name_indexing(const char* name, Expr* rhs){
    Expr* e = init_expr(Ast::EXPR_COMPOUND_NAME);
    e->name_indexing.name = Core::cstr(name);
    e->name_indexing.rhs = rhs;
    return e;
}
Expr* expr_new(){
    assert(false && "unimplemented NEW expr");
}
Expr* expr_unary(Lexer::tokenKind unary_kind, Expr* expr){
    assert(expr);
    Expr* e = init_expr(Ast::EXPR_UNARY);
    e->unary.unary_kind = unary_kind;
    e->unary.expr = expr;
    return e;
}
Expr* expr_binary(Lexer::tokenKind tok_kind, Expr* lhs, Expr* rhs){
    Expr* e = init_expr(EXPR_BINARY);
    e->binary.binary_kind = tok_kind;
    e->binary.left = lhs;
    e->binary.right = rhs;
    return e;
}
Expr* expr_cast(TypeSpec* typespec, Expr* expr){
    Expr* e = init_expr(Ast::EXPR_CAST);
    e->cast.typespec = typespec;
    e->cast.expr = expr;
    return e;
}
Expr* expr_ternary(Expr* cond, Expr* ifc, Expr* elsec){
    Expr* e = init_expr(Ast::EXPR_TERNARY);
    e->ternary.cond = cond;
    e->ternary.if_case = ifc;
    e->ternary.else_case = elsec;
    return e;
}
Expr* expr_assign(const char* name, TypeSpec* ts, Expr* rhs){
    Expr* e = init_expr(EXPR_INIT_VAR);
    e->init_var.name = Core::cstr(name);
    e->init_var.type = ts;
    e->init_var.rhs = rhs;
    return e;
}
Expr* expr_array_index(Expr* arr, Expr* index){
    Expr* e = init_expr(EXPR_ARRAY_INDEX);
    e->array.base = arr;
    e->array.index = index;
    return e;
}
Expr* expr_call(Expr* base, SVec<Expr*> args){
    Expr* e = init_expr(EXPR_CALL);
    e->call.base = base;
    e->call.args = args;
    return e;
}
SwitchCase* switch_case(SVec<SwitchCasePattern*> patterns, SVec<Stmt*> block){
    SwitchCase* sc = arena_alloc<SwitchCase>();    
    sc->patterns    = patterns;
    sc->block       = block;
    return sc;
}

Expr* expr_switch(Expr* cond, SVec<SwitchCase*>   cases, bool has_default, SVec<Stmt*> default_case){    
    Expr* e = init_expr(EXPR_SWITCH);
    e->expr_switch.cond = cond;
    e->expr_switch.cases = cases;
    e->expr_switch.has_default = has_default;
    e->expr_switch.default_case = default_case;            
    return e;
}
Expr* expr_lambda(SVec<ProcParam*> params, TypeSpec* ret, SVec<Stmt*> block){
    Expr* e = init_expr(Ast::EXPR_LAMBDA);
    e->lambda.params    = params;
    e->lambda.ret       = ret;
    e->lambda.block     = block;
    return e;
}
Expr* expr_new(TypeSpec* type, Expr* size, SVec<Expr*> args){
    Expr* e = init_expr(EXPR_NEW);
    e->new_expr.size = size;
    e->new_expr.type = type;
    e->new_expr.args = args;
    return e;
}
TypeSpec* init_typespec(TypeSpecKind kind, Lexer::Token token){
    TypeSpec* ts = arena_alloc<TypeSpec>();    
    ts->kind = kind;        
    ts->token = token;
    ts->resolvedTy = type_unresolved();
    return ts;
}
TypeSpec* typespec_name(const char* name, Lexer::Token token){
    TypeSpec* ts = init_typespec(TYPESPEC_NAME, token);
    ts->name = Core::cstr(name);
    return ts;
}
TypeSpec* typespec_pointer(TypeSpec* base, Lexer::Token token){
    TypeSpec* ts = init_typespec(Ast::TYPESPEC_POINTER, token);
    ts->base = base;
    return ts;
}
TypeSpec* typespec_array(TypeSpec* base, Expr* size, Lexer::Token token){
    TypeSpec* ts = init_typespec(TYPESPEC_ARRAY, token);
    ts->base = base;
    ts->array.size = size;
    return ts;
}
TypeSpec* typespec_proc(SVec<TypeSpec*> params, TypeSpec* ret, bool has_varags, Lexer::Token token){
    TypeSpec* ts = init_typespec(TYPESPEC_PROC, token);
    ts->proc.params     = params;
    ts->proc.ret        = ret;
    ts->proc.has_varags = has_varags;
    return ts;
}
TypeSpec* typespec_template(TypeSpec* typespec, TypeSpec* base, Lexer::Token token){
    TypeSpec* ts = init_typespec(TYPESPEC_TEMPLATE, token);
    ts->base = typespec;
    ts->templated = base;
    return ts;
}
TypeSpec* typespec_const(TypeSpec* base, Lexer::Token token){
    TypeSpec* ts = init_typespec(TYPESPEC_CONST, token);
    ts->base = base;
    return ts;
}
Stmt* init_stmt(StmtKind kind){
    Stmt* stmt = arena_alloc<Stmt>();    
    stmt->kind = kind;
    return stmt;
}

Stmt* stmt_expr(Expr* e){
    Stmt* stmt = init_stmt(STMT_EXPR);
    stmt->expr = e;
    return stmt;
}
Stmt* stmt_while(Expr* cond, SVec<Stmt*> block, bool is_doWhile){
    Stmt* st = init_stmt(STMT_WHILE);
    st->stmt_while = arena_alloc<StmtWhile>();
    st->stmt_while->cond = cond;
    st->stmt_while->block = block;
    st->stmt_while->is_doWhile = is_doWhile;
    return st;
}

IfClause* init_if_clause(Expr* cond, SVec<Stmt*> block){
    IfClause* ifc = arena_alloc<IfClause>();    
    ifc->cond = cond;
    ifc->block = block;
    return ifc;
}
Stmt* stmt_return(Expr* ret){
    Stmt* s = init_stmt(STMT_RETURN);
    s->expr = ret;
    return s;
}
Stmt* stmt_for(Expr* init, Expr* cond, Expr* inc, SVec<Stmt*> block){
    Stmt* st = init_stmt(Ast::STMT_FOR);    
    st->stmt_for.init = init;
    st->stmt_for.cond = cond;
    st->stmt_for.inc = inc;
    st->stmt_for.block = block;
    return st;
}
Stmt* stmt_if(IfClause* if_clause, SVec<IfClause*> elif_clauses, SVec<Stmt*> else_block){
    Stmt* st = init_stmt(STMT_IF);    
    st->stmt_if.if_clause       = if_clause;
    st->stmt_if.elif_clauses    = elif_clauses;
    st->stmt_if.else_block      = else_block;    
    return st;
}
Stmt* stmt_switch(Expr* cond, SVec<SwitchCase*>   cases, bool has_default, SVec<Stmt*> default_case){
    Stmt* st = init_stmt(Ast::STMT_SWITCH);
    st->stmt_switch.cond = cond;
    st->stmt_switch.cases = cases;
    st->stmt_switch.has_default = has_default;
    st->stmt_switch.default_case = default_case;    
    return st;
}

SwitchCasePattern* init_pattern(){
    SwitchCasePattern* pattern = arena_alloc<SwitchCasePattern>();
    pattern->begin  = 0;
    pattern->end    = pattern->begin;
    pattern->name   = 0;
    return pattern;
}
Decl* init_decl(DeclKind kind){
    Decl* d = arena_alloc<Decl>();
    d->kind = kind;
    return d;    
}
Decl* decl_var(const char* name, TypeSpec* type, Expr* init){    
    Decl* d = init_decl(DECL_VAR);
    d->name = Core::cstr(name);
    d->var.type = type;
    d->var.init = init;
    return d;
}    
ProcParam* proc_param(const char* name, TypeSpec* type, Expr* init){
    ProcParam* p    = arena_alloc<ProcParam>();
    p->name         = Core::cstr(name);
    p->type         = type;
    p->init         = init;
    p->isVararg     = false;
    return p;
}
ProcParam* proc_param_varargs(){
    ProcParam* p    = arena_alloc<ProcParam>();
    p->name         = Core::cstr("<variadic argument>");
    p->isVararg     = true;
    p->type         = typespec_name("any", {});
    return p;
}
Decl* decl_proc(const char* name, SVec<ProcParam*> params, TypeSpec* ret, SVec<Stmt*> block, bool is_complete, bool is_vararg = false, SVec<Note*> notes = {}, bool is_internal = false){
    Decl* d = init_decl(Ast::DECL_PROC);
    d->name = Core::cstr(name);
    d->proc.params          = params;
    d->proc.ret             = ret;
    d->proc.block           = block;
    d->proc.is_complete     = is_complete;    
    d->proc.is_vararg       = is_vararg;
    d->proc.is_internal     = is_internal;
    d->notes                = notes;
    return d;
}
Decl* decl_aggregate(const char* name, aggregateKind kind, SVec<AggregateItem*> items){
    assert(kind == AGG_STRUCT || kind == AGG_UNION);
    Decl* d = init_decl(DECL_AGGREGATE);
    d->name = Core::cstr(name);
    d->aggregate.aggkind = kind;
    d->aggregate.items = items;
    return d;
}
EnumItem* EnumItem_new(const char* name, Expr* init){    
    EnumItem* e = arena_alloc<EnumItem>();
    e->name = Core::cstr(name);
    e->init = init;
    return e;
}
Decl* decl_enum(const char* name, SVec<EnumItem*> items){
    Decl* d = init_decl(DECL_ENUM);
    d->name                 = Core::cstr(name);
    d->enumeration.items    = items;
    return d;
}
AggregateItem* aggregate_item_field(SVec<const char*> names, TypeSpec* type, Expr* init){
    AggregateItem* field = arena_alloc<AggregateItem>();
    field->kind = Ast::AGGREGATEITEM_FIELD;    
    field->field.names = names;
    field->field.type = type;
    field->field.init = init;
    return field;
}
Decl* decl_type(const char* name, TypeSpec* type){
    Decl* d = init_decl(DECL_TYPE);
    d->name = Core::cstr(name);
    d->type_alias.type = type;
    return d;    
}
Decl* decl_constexpr(const char* name, Expr* expr){
    Decl* d = init_decl(DECL_CONSTEXPR);
    d->name = Core::cstr(name);
    d->expr = expr;
    return d;
}
Decl* decl_use(SVec<Decl*> module, bool use_all, const char* rename){
    Decl* d = init_decl(DECL_USE);
    d->name = rename?: Core::cstr("<use>");
    d->use.module       = module;    
    d->use.use_all      = use_all;                
    d->use.rename       = rename;
    return d;
}
Decl* decl_impl(const char* target, SVec<Decl*> body){
    Decl* d = init_decl(Ast::DECL_IMPL);
    d->name = strf("<Impl: %s>", target);
    d->impl.target = target;
    d->impl.body = body;
    return d;
}

}

void tok_err(Pietra::Lexer::Token token, const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "%s: %i: %i: ", token.pos.filename, token.pos.line, token.pos.col);
    vprintf(fmt, ap);
    va_end(ap);
}
#endif /*AST_CPP*/  