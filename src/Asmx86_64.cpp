#ifndef ASMPLANG
#define ASMPLANG
#include "../include/Asmx86_64.hpp"
#include "pprint.cpp"
#include "resolve.cpp"
#include "ast.cpp"
#include "bridge.cpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>

#define DEBUG_MODE  false
#if DEBUG_MODE
#   define __OUT stdout
#else 
#   define __OUT ctx.OUT
#endif

#define printf(...) fprintf(__OUT, __VA_ARGS__)
#define err(...) fprintf(stderr, __VA_ARGS__)
#define printc(...) printf("\t"); printf(__VA_ARGS__)
#define println(...) printf("\t"); printf(__VA_ARGS__); printf("\n");
#define printlb(...) printf(__VA_ARGS__); printf(":\n")

namespace Pietra::Asm {

using namespace Pietra;
using namespace Ast;


const char* argreg8[]  = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
const char *argreg16[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
const char *argreg32[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
#define MAX_ARGREG 7

X86Context ctx;
Decl* cp;
int buffer_size = 0;
const char* BUILTIN_DUMP = 
    "dump:\n"
    "\tpush  rbp\n"
    "\tmov   rbp, rsp\n"
    "\tsub   rsp, 64\n"
    "\tmov   QWORD  [rbp-56], rdi\n"
    "\tmov   QWORD  [rbp-8], 1\n"
    "\tmov   eax, 32\n"
    "\tsub   rax, QWORD  [rbp-8]\n"
    "\tmov   BYTE  [rbp-48+rax], 10\n"
    ".L2:\n"
    "\tmov   rcx, QWORD  [rbp-56]\n"
    "\tmov   rdx, -3689348814741910323\n"
    "\tmov   rax, rcx\n"
    "\tmul   rdx\n"
    "\tshr   rdx, 3\n"
    "\tmov   rax, rdx\n"
    "\tsal   rax, 2\n"
    "\tadd   rax, rdx\n"
    "\tadd   rax, rax\n"
    "\tsub   rcx, rax\n"
    "\tmov   rdx, rcx\n"
    "\tmov   eax, edx\n"
    "\tlea   edx, [rax+48]\n"
    "\tmov   eax, 31\n"
    "\tsub   rax, QWORD  [rbp-8]\n"
    "\tmov   BYTE  [rbp-48+rax], dl\n"
    "\tadd   QWORD  [rbp-8], 1\n"
    "\tmov   rax, QWORD  [rbp-56]\n"
    "\tmov   rdx, -3689348814741910323\n"
    "\tmul   rdx\n"
    "\tmov   rax, rdx\n"
    "\tshr   rax, 3\n"
    "\tmov   QWORD  [rbp-56], rax\n"
    "\tcmp   QWORD  [rbp-56], 0\n"
    "\tjne   .L2\n"
    "\tmov   eax, 32\n"
    "\tsub   rax, QWORD  [rbp-8]\n"
    "\tlea   rdx, [rbp-48]\n"
    "\tlea   rcx, [rdx+rax]\n"
    "\tmov   rax, QWORD  [rbp-8]\n"
    "\tmov   rdx, rax\n"
    "\tmov   rsi, rcx\n"
    "\tmov   edi, 1\n"
    "\tmov   rax, 1\n"
    "\tsyscall\n"
    "\tnop\n"
    "\tleave\n"
    "\tret\n"
    ;


int count(){
    static long int __label_count = 0;
    return __label_count++;
}
void makeLabel(int addr){
    printlb(".L%i", addr);   
}
void makeLabel() {
    makeLabel(count());
}

    Type* compile_unary(Lexer::tokenKind kind, Expr* e, CState& state){
        switch(kind){  
            case Lexer::TK_NOT: {
                Type* ty = compile_expr(e, state);
                cmp_zero(ty);
                println("mov rcx, 0");
                println("mov rdx, 1");                
                println("cmove rcx, rdx");
                println("mov rax, rcx");
                
                return ty;
            }                      
            case Lexer::TK_MULT:    {
                Type* ty = compile_expr(e, state);
                assert(ty->kind == TYPE_PTR) ;
                load(ty->base);                
                return ty->base;                
            }          

            case Lexer::TK_AMPERSAND:   {                
                Type* type = compile_expr(e, state_getaddr);
                return type_ptr(type);
            }  

            case Lexer::TK_SUB: {
                Type* t = compile_expr(e, state);
                println("neg rax");
                return t;
            }
            default: assert(0);
        }
    }
    Type* compile_binary(Lexer::tokenKind kind, Ast::Expr* lhs, Ast::Expr* rhs, CState& state){        
        switch(kind){
            case Lexer::TK_EQ:  {                
                Type* lhs_t = compile_expr(lhs, state_getaddr);
                push("rax", lhs_t);                
                compile_expr(rhs, state);
                store();
                                
                return lhs_t;
            }         

            case Lexer::TK_MOD: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);
                Type* lhs_t = compile_expr(lhs, state);
                pop("rbx");
                println("xor rdx, rdx");
                println("div rbx");     
                println("mov rax, rdx");
                return lhs_t;
            }
            case Lexer::TK_DIV: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);
                Type* lhs_t = compile_expr(lhs, state);
                pop("rbx");
                println("xor rdx, rdx");
                println("div rbx");     
                return lhs_t;
            }
            case Lexer::TK_MULT:    {
                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);
                compile_expr(rhs, state);                
                pop("rbx");
                println("mul rbx");
                return lhs_t;
            }

            case Lexer::TK_CMPEQ:   {
                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);
                compile_expr(rhs, state);
                println("mov rbx, rax");
                pop("rcx");
                                 
                println("mov rdx, 1");       // rdx = 1
                println("xor rax, rax");     // rax = 0
                println("cmp rbx, rcx");     // rbx == rcx?                
                println("cmove rax, rdx")    // if true then rax = rdx, wich is 1.
                return type_int(8);
            }
            case Lexer::TK_ADD: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);
                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);

                Type* ret = pop("rax");
                pop("rbx");
                println("add rax, rbx");
                return ret;
            }
            case Lexer::TK_SUB: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);

                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);

                pop("rax");
                pop("rbx");
                println("sub rax, rbx");
                return lhs_t;
                
            }
            
        
            case Lexer::TK_PIPE:    {
                Type* rhs_t = compile_expr(rhs, state);                
                push("rax", rhs_t);
                Type* type = compile_expr(lhs, state);
                                
                pop("rbx");
                println("or rax, rbx");
                
                return type;
                
            };
            case Lexer::TK_LT:  {
                // lhs < rhs
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);                
                Type* lhs_t = compile_expr(lhs, state);
                pop("rbx");                

                // rax = lhs
                // rbx = rhs
                println("mov rcx, 0");
                println("mov rdx, 1");
                println("cmp rax, rbx");
                println("cmovl rcx, rdx");
                println("mov rax, rcx");
	        
                return lhs_t;
            }            
        }
        pPrint::expr(Utils::expr_binary(kind, lhs, rhs));
        exit(1);
    }
   
    CProc* get_cp(){
        assert(cp);
        CProc* p = GetProc(cp->name);
        assert(p);
        return p;
    }
    Type* lea(CVar* var){
        if(var->isGlobal){
            println("lea rax, G%s", var->name);
            return type_ptr(var->type);
        }
        else {
            println("lea rax, %s [rbp - %i]", 
                get_word_size(var->type->size),
                var->stackOffset);            
            return type_ptr(var->type);
        }
    }

    Type* mov(CVar* var){
        if(var->isGlobal){
            println("mov rax, G%s", var->name);
            return type_ptr(var->type);
        }
        else {
            println("mov rax, %s [rbp - %i]", 
                get_word_size(var->type->size),
                var->stackOffset);            
            return type_ptr(var->type);
        }
    }
    const char* get_word_size(int size){
        switch(size){            
            case 1:     return "BYTE";
            case 2:     return "WORD";
            case 4:     return "DWORD";
            default:    return "QWORD";
        }        
    }
    const char* rax_reg_from_size(int size){
        switch(size){            
            case 1:     return "ax";
            case 2:     return "ax";
            case 4:     return "eax";
            default:    return "rax";
        }
    }
    const char* rdi_reg_from_size(int size){
        switch(size){            
            case 1:     return "di";
            case 2:     return "di";
            case 4:     return "edi";
            default:    return "rdi";
        }
    }
    Type* load_word_size_from_stack(CVar* var, int offset){
        println("mov rax, %s [rbp - %i]", get_word_size(var->type->size), offset);
        return var->type;
    }
    std::vector<Type*> stack;
    void push(const char* what, Type* type){
        println("push %s", what);
        stack.push_back(type);
    }
    
    Type* pop(const char* target){
        assert(stack.size() > 0);
        println("pop %s", target);
        Type* type = stack.back();
        stack.pop_back();
        return type;
    }
    Type* cmp_zero(Type* ty){    
        switch (ty->kind) {
        case TYPE_I64:
        case TYPE_PTR:
        default:
            println("cmp rax, 0");
            return type_int(8);                            
        }
    }      
                            
    void store(){        
        Type* ty = pop("rdi");

        switch(ty->kind){
            default:
                println("mov [rdi], rax");
        }
    }
    
    Type* load(Type* type){        
        switch(type->kind){
        case TYPE_I8:
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64:  
        case TYPE_ANY: 
        default: {
            const char* word = get_word_size(type->size);
            const char* inst = "movs";
            if(type->size == 1){
	            println("%sx rax, %s [rax]\n", 
                inst,
		        word);
            } else if (type->size == 2){
	            println("%sx rax, %s [rax]\n", 
                inst,
		        word);
            }
            else if (type->size == 4){
	            println("%sxd rax, %s [rax]\n",
                inst,
		        word);
            }
            else {
    	        println("mov rax, %s [rax]\n",
		        word);
            }
            return type;
        }
        case TYPE_PTR:        
            println("mov rax, [rax]");
            return type->base;
        
        case TYPE_F32:
        case TYPE_F64:                        
        case TYPE_NONE:
        case TYPE_UNRESOLVED:
        case TYPE_VOID:        
        case TYPE_FIRST_ARITHMETRIC_TYPE:        
        case TYPE_LAST_ARITHMETRIC_TYPE:        
        case TYPE_ARRAY:
        case TYPE_STRUCT:
        case TYPE_UNION:
        case TYPE_PROC:
        case TYPE_CONST: 
            println("mov rax, [rax]");
            return type->base;
        }
    }
    Type* load(CVar* var, CState& state){
        if(state != state_getaddr){
            return lea(var);            
        }
        return load(var->type);
    }
    Type* compile_init_var(const char* name, Type* type, Expr *init, bool isGlobal, CState state){        
        if(state != state_none){
            err("[ERR]: init var with state != state_none.\n");
            exit(1);
        }

        if(isGlobal){
            assert(0 && "global variables should be preprocessed in the resolver");
        }
        else {
            assert(cp);
            CProc* proc = GetProc(cp->name);
            assert(proc);
            CVar* var = proc->find_param(name);
            if(!var){
                var = proc->find_local(name);
            }
            

            if(!var){
                err("------------------------\n");
                for(CVar* v: *proc->locals){
                    err("inside %s got locals = %s\n", proc->name, v->name);
                }
                for(CVar* v: *proc->params){
                    err("inside %s got params = %s\n", proc->name, v->name);
                }
                
                err("[EROR]: could not find the variable: '%s'\n", name);
                exit(1);
            }

            if(init){            
                lea(var);                
                push("rax", var->type);
                compile_expr(init, state_none);
                store();                
            }
            return var->type;

        }
    }

    Type* compile_call(Expr* base, SVec<Expr*> args, CState& state){            
        if(base->kind == EXPR_NAME){
            if(base->name == Core::cstr("asm")){
                println(";; NOTE: inline asm here.\n");
                for(Expr* arg: args){
                    if(arg->kind != Ast::EXPR_STRING){
                        compile_expr(arg, state_none);
                    }
                    else {
                        const char* str = arg->string_lit;
                        println("%s", str);
                    }
                }               
                return type_any();
            }
            // TODO: move sizeof to resolve.cpp
            if(base->name == Core::cstr("sizeof")){
                assert(args.len() == 1);
                Expr* arg = args.at(0);
                if(arg->kind != EXPR_NAME){
                    err("[ERROR]: sizeof expects TOKEN NAME.\n");
                    exit(1);
                }

                Sym* sym = sym_get(arg->name);
                if(!sym){
                    err("[ERROR]: the name %s is not defined.\n", arg->name);
                    exit(1);
                }

                Expr* e;
                if(sym->kind == Resolver::SYM_TYPE or sym->kind == Resolver::SYM_AGGREGATE or sym->kind == Resolver::SYM_VAR){
                    e = Utils::expr_int(sym->type->size);
                }
                                
                else {                    
                    err("[ERROR]: could not resolve sizeof %s\n", sym->name);
                    exit(1);
                }

                if(!e){
                    err("[ERROR]: compiling sizeof got null expression.\n");
                    assert(0);
                }

                return compile_expr(e, state_none);
            }
    
            if(base->name == Core::cstr("syscall")){
                int len = args.len();
                if(len > 1){
                    assert(len <= MAX_ARGREG);
                    int id = 0;
                    
                    for(int i = len - 2; i >= 0; i--){
                        Expr* arg = args.at(i);
                        compile_expr(arg, state);                    
                        println("mov %s, rax", argreg64[id++]);
                        
                    }
                }
                
                compile_expr(args.back(), state);
                println("syscall");
                return type_int(64);
            }
            else if(base->name == Core::cstr("dump")){
                assert(args.len() == 1);
                compile_expr(args.at(0), state);
                println("mov rdi, rax");
                println("call dump");
                return type_void();
            }
            else if(base->name == Core::cstr("__builtin_buffer_begin")){
                assert(args.len() == 0);
                println("mov rax, buffer");
                return type_ptr(type_void());
            }
            else if(base->name == Core::cstr("__builtin_buffer_alloc")){
                if(args.len() != 1){
                    err("[ERR]: __builtin_get_buffer expects 1 argument.\n");
                    exit(1);
                }
                if(args.at(0)->kind != EXPR_INT){
                    err("__builtin_buffer_alloc expects a int size.\n");
                    exit(1);
                }
                if(buffer_size > 0){
                    println("mov rax, buffer + %i", buffer_size);
                }
                else {
                    println("mov rax, buffer");
                }
                buffer_size += args.at(0)->int_lit;
                return type_ptr(type_void());
            }
        }
        {   
            assert(args.len() < MAX_ARGREG);
            int id = 0;
            for(Expr* arg: args) {
                compile_expr(arg, state);
                println("mov %s, rax", argreg64[id++]);
            }
        }

        Type* base_t = compile_expr(base, state);
        println("call rax");
        return base_t->proc.ret_type;
        
    }
    Type* compile_name(const char* name, CState& state){                
        // Check for procedures
        if(CProc* cp = GetProc(name)){            
            println("mov rax, %s", cp->name);            
            assert(cp->type);                        
            return cp->type;
        }        
        
        // Check for globals
        if(CVar* global = find_global(name)){
            lea(global);
            if(state != state_getaddr){
                load(global->type);
            }
            return global->type;
        }

        // Check for parameters
        
        if(cp){
            CProc* proc = GetProc(cp->name);
            assert(proc);
            if(CVar* param = proc->find_param(name)){
                makeLabel();
                lea(param);
                if(state != state_getaddr){
                    load(param->type);
                }
                return param->type;
            }
            if(CVar* local = proc->find_local(name)){
                makeLabel();
                lea(local);                                     
                if(state != state_getaddr){                
                    load(local->type);                    
                }
                
                return local->type;
            }      
            if(CConstexpr* ce = CBridge::find_constexpr(name)){
                return compile_expr(ce->expr, state);
            }
        }            
        err("[ERR]: name '%s' not found in this scope.\n", name);
        exit(1);
    }

    Type* compile_index_offset(Type* ty, CState& state){
        // Here we expects 1 element on the stack that is the pointer to the type
        // the index must be in the rax register
        assert(ty->base);
        if (ty->base->size > 1){
            println("mov rdx, %i", ty->base->size);
            println("mul rdx");
            pop("rbx");
            println("add rax, rbx");
        }
        else {
            pop("rbx");            
            println("add rax, rbx");               
        }

        if(state != state_getaddr){
            return load(ty->base);
        }
        else {
            return ty;
        }
    }
    Type* compile_index(Expr* base, Expr* index, CState& state){
        Type* tb = compile_expr(base, state_none);
        if(!tb->base){
            err("[ERROR]: Tring to get the index of a non-pointer type.\n");
            exit(1);
        }
        push("rax", tb);

        Type* in = compile_expr(index, state_none);                
        int base_sz = tb->base->size;
        // TEST: return compile_index_offset(tb, state);

        if(base_sz > 1){            
            println("mov rdx, %i", base_sz);
            println("mul rdx");
            pop("rbx");
            println("add rax, rbx");
        }
        else {                         
            pop("rbx");            
            println("add rax, rbx");               
        }
                
        if(state != state_getaddr){
            load(tb->base);
        }

        
        return tb->base;
    }
    Type* compile_field(Expr* parent, Expr* children, CState& state){    
        Type* p = compile_expr(parent, state_getaddr);
        if(p->kind == Ast::TYPE_PTR){
            if(p->base->kind == Ast::TYPE_STRUCT){
                load(p);
                p = p->base;        
            }
        }        
                
        
        if(children->kind == Ast::EXPR_NAME){
            // Search for Impl methods     
            if(Sym* sym = sym_get(p->name)){
                SymImpl& impls = sym->impls;
                const char* target = Core::cstr(strf("%s_impl_%s", p->name, children->name));
                if(Sym* impl = impls.find(target)){
                    assert(impl->kind == SYM_PROC);
                    println("mov rax, %s", impl->name);
                    return impl->type->proc.ret_type;
                }
            }        
            if(p->kind == TYPE_STRUCT){
                size_t offset = 0;
                for(TypeField* field: p->aggregate.items){
                    if(field->name != children->name){
                        offset += field->type->size;                    
                        continue;
                    }

                    if(offset > 0){
                        // rax: lea 
                        println("add rax, %zu", offset);
                    }
                    if(state != state_getaddr){
                        load(p);
                    }
                    return field->type;
                }
            }            
            err("target = %s\n", p->name);

            err("[ERROR]: field %s is not existent in the object %s.", children->name, p->repr());
            exit(1);            
        }
        else {
            err("[ERR]: get field children must be <str>.\n");
            exit(1);
        }
        println("SEAOKDOPASKDOPASKDop\n");
        err("TODO: %s\n", __func__);
        exit(1);
        
    }
    Type* compile_cast(TypeSpec* ts, Expr* expr, CState& state){
        assert(ts->resolvedTy);
        Type* ty = ts->resolvedTy;
        compile_expr(expr, state);
        return ty;
    }
    Type* compile_expr(Expr* e, CState& state){                
        static int i = 0;
        
        switch(e->kind){
            case EXPR_INT:                
                makeLabel();
                println("mov rax, %zu", e->int_lit);
                return type_int(64);
            case EXPR_STRING:   {
                makeLabel();
                int str_id = CreateGlobalString(e->string_lit);
                println("mov rax, S%i", str_id);                
                return type_string();
            }                
            
            case EXPR_NAME:     return compile_name(e->name, state);
            case EXPR_UNARY:    return compile_unary(e->unary.unary_kind, e->unary.expr, state);
            case EXPR_BINARY:   return compile_binary(e->binary.binary_kind, e->binary.left, e->binary.right, state);
            case EXPR_INIT_VAR:
                return compile_init_var(
                    e->init_var.name,
                    e->init_var.type->resolvedTy,
                    e->init_var.rhs,
                    false,
                    state
                );

            case EXPR_CALL:  return compile_call(e->call.base, e->call.args, state);
            case EXPR_CAST:  return compile_cast(e->cast.typespec, e->cast.expr, state);                
            case EXPR_ARRAY_INDEX:  return compile_index(e->array.base, e->array.index, state);
            case EXPR_FIELD:        return compile_field(e->field_acc.parent, e->field_acc.children, state);
            default:                 
                fprintf(stderr, "========================\n");
                pPrint::expr(e);
                fprintf(stderr, "========================\n");
                exit(1);
        }
    }

  void compile_stmt(Stmt* stmt){

        switch(stmt->kind){
            case Ast::STMT_EXPR:    {
                compile_expr(stmt->expr, state_none);                
            } break;
            case Ast::STMT_FOR: {
                if(!stmt->stmt_for.init and !stmt->stmt_for.cond and !stmt->stmt_for.inc){
                    // Infinite loop
                    int begin = count();
                    makeLabel(begin);
                    compile_block(stmt->stmt_for.block);
                    println("jmp .L%i", begin);
                    break;
                }

                if(stmt->stmt_for.init){
                    static int for_depth = 0;
                    Expr* init = stmt->stmt_for.init;
                    assert(init->kind == Ast::EXPR_INIT_VAR);
                    assert(init->init_var.type);
                    assert(init->init_var.type->resolvedTy);
                    
                    CProc* proc = get_cp();
                    CVar* tmp_var = proc->init_tmp_var(init->init_var.name, init->init_var.type->resolvedTy);
                    for_depth++;                    
                    {
                        int end = count();
                        err("init offset = %i\n", tmp_var->stackOffset);
                    }
                    for_depth--;

                    if(for_depth == 0){
                        proc->forget_tmp_vars();
                    }

                    exit(1);
                }

                assert(0);
            };
            case Ast::STMT_IF:  {                
                int if_end   = count();
                {
                    Type* cond = compile_expr(stmt->stmt_if.if_clause->cond, state_none);
                    cmp_zero(cond);
                    println("je .L%i", if_end);
                    compile_block(stmt->stmt_if.if_clause->block);
                    
                }
                if(stmt->stmt_if.elif_clauses.len() > 0){
                    makeLabel(if_end);
                    if_end = count();
                    for(IfClause* elif: stmt->stmt_if.elif_clauses){
                        int elif_end = count();                        
                        
                        Type* cond = compile_expr(elif->cond, state_none);
                        cmp_zero(cond);
                        println("je .L%i", elif_end);
                        compile_block(elif->block);                        
                        makeLabel(elif_end);
                    }
                }
                {
                    if(stmt->stmt_if.else_block.len() > 0){
                        int else_end = count();
                        println("jmp .L%i", else_end);

                        makeLabel(if_end);
                        compile_block(stmt->stmt_if.else_block);
                        makeLabel(else_end);
                    }
                    else {
                        makeLabel(if_end);
                    }
                }

                
            } break;

            case Ast::STMT_WHILE:   {
                int begin = count();
                int end   = count();

                makeLabel(begin);
                Type* ty = compile_expr(stmt->stmt_while->cond, state_none);
                cmp_zero(ty);
                println("je .L%i", end);
                compile_block(stmt->stmt_while->block);
                println("jmp .L%i", begin);

                makeLabel(end);
            } break;
            case Ast::STMT_RETURN:  {
                CProc* proc = get_cp();
                compile_expr(stmt->expr, state_none);
                println("jmp .PE.%s", proc->name);
              
            } break;
            default: assert(0);
        }
    }
    void compile_block(SVec<Stmt*> block){
        for(Stmt* stmt: block) 
            compile_stmt(stmt);
    }
    
    void compile_cproc_params(CProc *cp, SVec<ProcParam*> params){        
         //TODO: i guess
    }
    void compile_decl_proc(Decl* d){        
        if(d->proc.is_internal) return;
        if(d->name == Core::cstr("dump")) return;

        CProc* proc = GetProc(d->name);
        cp = d;
        assert(proc);
        compile_cproc_params(proc, d->proc.params);

        printf("%s:\n", proc->name);
        printlb(".PB.%s", proc->name);
        println("push rbp");
        println("mov rbp, rsp");        

        if(proc->stackAllocation > 0){
            println("sub rsp, %zu", proc->stackAllocation);
        }

        assert(proc->params->len() < MAX_ARGREG);
        {
            int id = 0;
            for(CVar* param: *proc->params){
                println("mov [rbp - %i], %s", param->stackOffset, argreg64[id++]);
            }
        }
        compile_block(d->proc.block);        
        println("mov rax, 0");
        printlb(".PE.%s", proc->name);
        println("leave");
        println("ret");                
    }
    void compile_decl_impl(Decl* &decl){        
        for(Decl* node: decl->impl.body){
            assert(node->kind == DECL_PROC);                        
            compile_decl_proc(node);
        }                    
    }
    void compile_decl(Decl* decl){        
        switch(decl->kind){
            case DECL_PROC: 
                compile_decl_proc(decl); 
                break;
            case DECL_VAR:          break;
            case DECL_CONSTEXPR:    break;                                            
            case DECL_AGGREGATE:    break;
            case DECL_TYPE:         break;
            case DECL_USE:          break;
            case DECL_ENUM:         break;
            case DECL_IMPL:            
                compile_decl_impl(decl);
                break;
            default: 
                err("[ERROR]: CANT COMPILE THIS...............\n");
                pPrint::decl(decl);
                exit(1);
        }
    }

    void compile_segment_data(){                
        if(Strs.len() > 0){
            printf("segment .data\n");
            int id = 0;
            for(std::string str: Strs){
                printc("S%i: db ", id++);
                for(char c: str){
                    printf("%zu, ", (size_t)c);
                }
                printf("0\n");
            }
            
        }
    }
    void compile_segment_bss(){
        printf("segment .bss\n");
        println("__heap_end__: resq 1");

        if(buffer_size > 0 or globals.len() > 0){            
            if(buffer_size > 0){
                println("buffer: resb %i", buffer_size);
            }
            for(CVar* global: globals){
                    printc("G%s: resb %i\n", global->name, global->type->calcSize());
            }
        }
    }

    void compile_ast(SVec<Decl*> ast){                        
        ctx.OUT = fopen("pietra.asm", "w");
        assert(ctx.OUT);

        printf("BITS 64\n");
        printf("segment .text\n");
        println("global _start:");

        printf("%s", BUILTIN_DUMP);
        
        for(Decl* decl: ast){
            compile_decl(decl);
        }


        printlb("_start");
        //CProc* pmain = GetProc("main");
//
        //if(!pmain){
        //    err("[ERROR]: where is main?\n");
        //    exit(1);
        //}        
        // Initialize allocator __heap_end__
        println("mov rax, 12");
        println("mov rdi, 0");
        println("syscall");        
        println("mov [__heap_end__], rax")
        for(CVar* global: globals){
            assert(global->isGlobal);
            if(global->init){
                lea(global);
                push("rax", global->type);
                compile_expr(global->init, state_none);
                store();
            }
        }
        //if(pmain->params.len() != 0)
        {
            //assert(pmain->params.len() == 2);
            println("mov %s, [rsp]", argreg64[0]);
            println("mov %s, rsp", argreg64[1]);
            println("add %s, 8\n", argreg64[1]);
            
        }
        println("call main");
        println("mov rax, 60");
        println("mov rdi, 0");
        println("syscall");
        compile_segment_data();
        compile_segment_bss();
                
        fclose(ctx.OUT);

        if(!DEBUG_MODE){
            system(strf("nasm -felf64 pietra.asm"));
            system(strf("ld pietra.o -o pietra.bin"));
            system(strf("rm pietra.o"));
        }
    }
    
}

#undef printf
#endif /*ASMPLANG*/