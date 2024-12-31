#ifndef ASMPLANG
#define ASMPLANG
#include "../include/Asmx86_64.hpp"
#include "context.cpp"

#define VARIADIC_ARGS_CAP 4096

const char* AGGREGATE_CONSTRUCTOR_WORD  = Core::cstr("constructor");
const char* AGGREGATE_DELETER_WORD      = Core::cstr("delete");
const char* keyword_asm                 = Core::cstr("asm");
const char* keyword_syscall             = Core::cstr("syscall");
const char* keyword_warn                = Core::cstr("warn");
const char* keyword_extern              = Core::cstr("extern");
const char* keyword_main                = Core::cstr("main");

#define DEBUG_MODE  false
#if DEBUG_MODE
#   define __OUT stdout
#   define P_SET_CTXOUT(filename) 
#else 
#   define __OUT ctx.outputChannel

#   define P_SET_CTXOUT(filename)               \
    ctx.outputChannel = fopen(filename, "w");   \
    assert(ctx.outputChannel);
#endif

#undef  printf
#define printf(...)     fprintf(ctx.outputChannel, __VA_ARGS__)
#define printc(...)     printf("\t"); printf(__VA_ARGS__)
#define println(...)    printf("\t"); printf(__VA_ARGS__); printf("\n");
#define printlb(...)    printf(__VA_ARGS__); printf(":\n")
#define err(...)        fprintf(stderr, __VA_ARGS__)

namespace Pietra::Asm {
    using namespace Resolver;

using namespace Pietra;
using namespace Ast;

std::vector<Type*> stack;
bool has_extern = false;
const char* extern_paths = "";
const char* argreg8 [] = {"dil", "sil", "dl",  "r10b", "r8b", "r9b"};
const char *argreg16[] = {"di",  "si",  "dx",  "r10w", "r8w", "r9w"};
const char *argreg32[] = {"edi", "esi", "edx", "r10d", "r8d", "r9d"};
const char *argreg64[] = {"rdi", "rsi", "rdx", "r10",  "r8",  "r9"};
#define MAX_ARGREG 7


int buffer_size = 0;
SVec<Sym*> post_declared_compile;


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

    Type* compile_unary(TokenKind kind, Expr* e, State::CompilerState& state){
        switch(kind){  
            case TK_not: {
                Type* ty = compile_expr(e, state);
                cmp_zero(ty);
                println("mov rcx, 0");
                println("mov rdx, 1");                
                println("cmove rcx, rdx");
                println("mov rax, rcx");
                
                return ty;
            }                      
            case TK_mul:    {
                Type* ty = compile_expr(e, state);
                assert(ty->kind == TYPE_PTR) ;
                load(ty->base);                
                return ty->base;                
            }          

            case TK_and:   {                
                Type* type = compile_expr(e, State::CompilerState::GetAddress());
                return type_ptr(type, type->ismut);
            }  

            case TK_minus: {
                Type* t = compile_expr(e, state);
                println("neg rax");
                return t;
            }
            
            default: 
                err("Can't compile unary for %s\n", tokenKindRepr(kind));
                exit(1);                
        }
    }
    Type* compile_eq(Expr* lhs, Expr* rhs, State::CompilerState& state){
        Type* lhs_t = compile_expr(lhs, State::CompilerState::GetAddress());                        
        if(lhs_t->name){
            if(Sym* sym = sym_get(lhs_t->name)){                
                // NOTE:
                /*
                    if method __eq__ exist the compiler will call it even if struct_reassigner exists
                    struct_reassigner will only be called if:
                        struct_reassigner exists
                        __eq__ isn't defined
                    otherwise will print an error to the screen
                */
                if(sym->kind == Resolver::SYM_AGGREGATE){
                    if(Sym* __eq = sym->impls.find("__eq__")){
                        push("rax", lhs_t); 
                        compile_expr(rhs, State::CompilerState::None());
                        println("mov %s, rax\n", argreg64[1]);
                        pop(argreg64[0]);
                        // Get the addr and call the method (lhs).__eq__                    
                        println("call %s", __eq->name);                    
                        return lhs_t;
                    }                    
                    else if (struct_reassigner){
                        // call function(dst, src, dst.size)        
                        int dst_size = sym->type->size;
                        Expr* base = Utils::expr_name(lhs->loc, STRUCT_REASSIGN_FLAG);
                        SVec<Expr*> args;
                        
                        args.push(Utils::expr_unary(lhs->loc, TK_and, lhs));
                        args.push(Utils::expr_unary(lhs->loc, TK_and, rhs));                                                                        
                        args.push(Utils::expr_int(lhs->loc, dst_size));
                        Expr* call = Utils::expr_call(lhs->loc, base, args);
                        println(";; Calling std.config `%s`", struct_reassigner->name);

                        // NOTE: this will be compiled to struct_reassign(lhs, rhs, sizeof(lhs))
                        return compile_expr(call, State::CompilerState::GetAddress());                        
                    }
                    else {
                        // Call std.memcpy 
                        err("[ERROR]: Struct `%s` can't be assigned directly without prep-proc: `%s` or method implementation __eq__.\n",                             
                            sym->type->repr(),
                            STRUCT_REASSIGN_FLAG);
                        exit(1);
                    }
                }
            }

        }        
        push("rax", lhs_t);        
        Type* rhs_t = compile_expr(rhs, state);        
        if(rhs_t->kind == TYPE_STRUCT or rhs_t->kind == TYPE_UNION){
            rhs_t = compile_expr(rhs, State::CompilerState::GetAddress());
        }
        store(lhs_t, rhs_t);        
        return lhs_t;
            
    }
    Type* compile_binary(TokenKind kind, Ast::Expr* lhs, Ast::Expr* rhs, State::CompilerState& state){        
        switch(kind){            
            case TK_andand:{
                int end_fail    = count();
                int end_ok      = count();
                int end         = count();


                Type* lhs_t = compile_expr(lhs, State::CompilerState::None());
                cmp_zero(lhs_t);
                println("je .L%i", end_fail);
                Type* rhs_t = compile_expr(rhs, State::CompilerState::None());
                cmp_zero(rhs_t);
                println("je .L%i", end_fail);

                makeLabel(end_ok);
                println("mov rax, 1");
                println("jmp .L%i", end);
                makeLabel(end_fail);                
                println("mov rax, 0");
                makeLabel(end);

                return lhs_t;
            }
            case TK_oror: {
                Type* lhs_t = compile_expr(lhs, State::CompilerState::None());
                cmp_zero(lhs_t);
                push("rax", lhs_t);
                Type* rhs_t = compile_expr(rhs, State::CompilerState::None());
                cmp_zero(rhs_t);
                pop("rbx");
                println("add rax, rbx");                       
                return lhs_t;
            }
            case TK_eq: return compile_eq(lhs, rhs, state);                      

            case TK_mod: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);
                Type* lhs_t = compile_expr(lhs, state);
                pop("rbx");
                println("xor rdx, rdx");
                println("div rbx");     
                println("mov rax, rdx");
                return lhs_t;
            }
            case TK_div: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);
                Type* lhs_t = compile_expr(lhs, state);
                pop("rbx");
                println("xor rdx, rdx");
                println("div rbx");     
                return lhs_t;
            }
            case TK_mul:    {
                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);
                compile_expr(rhs, state);                
                pop("rbx");
                println("mul rbx");
                return lhs_t;
            }
            case TK_noteq:
            case TK_eqeq:   {
                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);
                compile_expr(rhs, state);
                println("mov rbx, rax");
                pop("rcx");
                                 
                println("mov rdx, 1");       // rdx = 1
                println("xor rax, rax");     // rax = 0
                println("cmp rbx, rcx");     // rbx == rcx?                
                if(kind == TK_noteq){
                    println("cmovne rax, rdx");
                }
                else {
                    println("cmove rax, rdx");    // if true then rax = rdx, wich is 1.
                }
                return type_int(8, lhs_t->ismut);
            }
            case TK_plus: {
                // TODO OPTIONAL: optimize this section
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);
                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);

                
                if(lhs_t->name){
                    if(Sym* sym = sym_get(lhs_t->name)){
                        if(Sym* __add = sym->impls.find("__add__")){                            
                            // WARN_ONCE("[WARN]: TODO move structs without pass by addr.\n");
                            // @stack = rhs lhs
                            compile_expr(lhs, State::CompilerState::GetAddress());                                                                                                                 
                            push("rax", lhs_t);
                            compile_expr(rhs, State::CompilerState::GetAddress());                                                                                                                                             
                            println("mov %s, rax", argreg64[1]);
                            pop(argreg64[0]);
                            println("call %s", __add->name);                            
                            return __add->type->proc.ret_type;                            
                        }
                    }
                }
                {
                    Type* ret = pop("rax");
                    pop("rbx");
                    println("add rax, rbx");
                    return ret;
                }
            }
            case TK_minus: {
                Type* rhs_t = compile_expr(rhs, state);
                push("rax", rhs_t);

                Type* lhs_t = compile_expr(lhs, state);
                push("rax", lhs_t);

                pop("rax");
                pop("rbx");
                println("sub rax, rbx");
                return lhs_t;
                
            }
            
        
            case TK_or:    {
                Type* rhs_t = compile_expr(rhs, state);                
                push("rax", rhs_t);
                Type* type = compile_expr(lhs, state);
                                
                pop("rbx");
                println("or rax, rbx");
                
                return type;
                
            };
            
            case TK_greater:              
            case TK_less:  
            case TK_lesseq:  {
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
                if(kind == TK_less){
                    println("cmovl rcx, rdx");
                }
                else if(kind == TK_greater) {                    
                    println("cmovg rcx, rdx");
                }
                else if(kind == TK_lesseq) {
                    println("cmovle rcx, rdx");
                }
                else {
                    err("Couldn't compile this binary.\n");                    
                    pPrint::expr(Utils::expr_binary(lhs->loc, kind, lhs, rhs));                    
                    exit(1);                    
                }
                println("mov rax, rcx");
	        
                return lhs_t;
            }                    
        }        

        err("[ERROR]: Couldn't compile the following expression.\n");
        pPrint::expr(Utils::expr_binary(lhs->loc, kind, lhs, rhs));
        exit(1);
    }
   
    
    Type* lea(Variables::Variable* var){
        if(var->isGlobal){
            println("lea rax, G%s", var->name);
            return type_ptr(var->type, var->type->ismut);
        }
        else {
            println("lea rax, %s [rbp - %i]", 
                get_word_size(var->type->size),
                var->stackOffset);            
            return type_ptr(var->type, var->type->ismut);
        }
    }

    Type* mov(Variables::Variable* var){
        if(var->isGlobal){
            println("mov rax, G%s", var->name);
            return type_ptr(var->type, var->type->ismut);
        }
        else {
            println("mov rax, %s [rbp - %i]", 
                get_word_size(var->type->size),
                var->stackOffset);            
            return type_ptr(var->type, var->type->ismut);
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
    Type* load_word_size_from_stack(Variables::Variable* var, int offset){
        println("mov rax, %s [rbp - %i]", get_word_size(var->type->size), offset);
        return var->type;
    }
    
    void push(const char* what, Type* type){
        println("push %s", what);
        stack.push_back(type);
    }
    Type* pop(){
        println("pop rax");
        stack.pop_back();
        return type_void();
    }
    Type* pop(const char* target){
        assert(stack.size() > 0);
        println("pop %s", target);
        Type* type = stack.back();
        stack.pop_back();
        return type;
    }

    Type* pop_nostack(const char* target){
        stack.push_back(type_void());
        return pop(target);
    }
    Type* cmp_zero(Type* ty){    
        switch (ty->kind) {
        case TYPE_I64:
        case TYPE_PTR:
        default:
            println("cmp rax, 0");
            return type_int(8, false);
        }
    }      
                            
    void store(){             
        Type* type = pop("rdi");        
        const char* word = get_word_size(type->size);        
        if(type->size == 1)
        {
            println("mov [edi], %s\n",                
                rax_reg_from_size(type->size));        
        }
        else {
            println("mov %s [rdi], %s\n",
                    word,
                    rax_reg_from_size(type->size));        
        }
    }

    void store(Type* lhs, Type* rhs){
        store();
        return;
        if(lhs->kind == rhs->kind and (lhs->kind == Ast::TYPE_STRUCT or lhs->kind == Ast::TYPE_UNION)){

            Type* lhs_t = pop("rdi");
            assert(lhs == lhs_t);
            size_t offset = 0;
            println(";; NOTE: Assign struct to struct");
            for(TypeField*& field: lhs->aggregate.items){
                println(";; NOTE: Assign struct %s, in field [size=%i] %s", lhs->name, field->type->size, field->name);
                // var rdi
                // val rax
                if(offset > 0){
                    println("mov rcx, [rax + %zu]", offset);
                }
                else {
                    println("mov rcx, [rax]");                    
                }
                {
                    println("mov %s [rdi], rcx", 
                        get_word_size(field->type->size));
                    println("add rdi, %i\n", field->type->size);
                }

                offset += field->type->size;                
            }
            return;          
        }
        
        store();
        
    }
    
    const char* reg_from_size(int size, int index){
        switch(size){
            case 1: return argreg8[index];
            case 2: return argreg16[index];
            case 4: return argreg32[index];
            case 8: return argreg64[index];                            
        }
        err("reg_from_sz idk error: size: %i\n", size);
        exit(1);
    }
    void mov_to_rax(Type* type, const char* reg){        
        switch(type->size){
            case 1:
                println("mov %s %s, rax", get_word_size(type->size), reg);
                return;
            case 2:
                println("mov %s %s, rax", get_word_size(type->size), reg);
                return;
            case 4:
                println("mov %s %s, rax", get_word_size(type->size), reg);
                return;
            case 8:
            default:
                println("mov %s %s, rax", get_word_size(type->size), reg);
                return;
        }

        assert(0);
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
    Type* load(Variables::Variable* var, State::CompilerState& state){
        if(state != State::CompilerState::GetAddress()){
            return lea(var);            
        }
        return load(var->type);
    }
    
    void compile_aggregate_constructor(Variables::Variable* var, Sym* constructor){
        // TODO: make the variable childs call for its constructor        
        Expr* base  = Utils::expr_name({}, var->name);                    
        compile_expr(base, State::CompilerState::None());
        println("mov rdi, rax");        
        Expr* e     = Utils::expr_field_access({}, base, Utils::expr_name({}, AGGREGATE_CONSTRUCTOR_WORD));
        compile_expr(e, State::CompilerState::None());                            
        println("call rax");        
    }

    Type* compile_init_var(const char* name, Type* type, Expr *init, bool isGlobal, State::CompilerState& state){
        assert(!isGlobal && "global variables should be preprocessed in the resolver");
        if(state != State::CompilerState::None()){
            err("[ERR]: init var with state != None.\n");
            exit(1);
        }
        
        Procedures::Procedure* proc = ctx.getCurrentProcedure();                
        assert(proc);
        //
        //Procedures::Procedure* proc = ctx.findProcedureByName(cp->name);
        //assert(proc);
        
        Variables::Variable* var = proc->findParameter(name);
        if(!var){
            var = proc->findLocalVariable(name);
        }
        

        if(!var){
            err("------------------------\n");
            for(Variables::Variable* v: *proc->localVariables){
                err("inside %s got locals = %s\n", proc->name, v->name);
            }
            for(Variables::Variable* v: *proc->parameters){
                err("inside %s got params = %s\n", proc->name, v->name);
            }
                        
            err("[EROR]: could not find the variable: '%s'\n", name);
            exit(1);            
        }        
        if(var->type->kind != TYPE_PTR ){            
            Sym* sym = sym_get(unqualify_type(var->type)->name);

            if(sym and sym->type->kind != TYPE_PTR and sym->kind == Resolver::SYM_AGGREGATE and sym->type->name){                                
                if(Sym* constructor = sym->impls.find(AGGREGATE_CONSTRUCTOR_WORD)){
                    compile_aggregate_constructor(var, constructor);
                }                
            }            
        }        
        if(init){                     
            Expr* eq = Utils::expr_binary(init->loc, TK_eq, Utils::expr_name(init->loc, var->name), init);            
            compile_expr(eq, State::CompilerState::None());                        
        }           
        return var->type;        
    }
    Type* compile_call(Expr* base, SVec<Expr*> args, State::CompilerState& state){                    
        if(base->kind == EXPR_NAME){            
            if(base->name == keyword_asm){
                println(";; NOTE: inline asm here.\n");
                for(Expr* arg: args){
                    if(arg->kind != Ast::EXPR_STRING){
                        compile_expr(arg, State::CompilerState::None());
                    }
                    else {
                        const char* str = arg->string_lit;
                        println("%s", str);
                    }
                }               
                return type_any();
            }                
            else if(base->name == Core::cstr("sizeof")){
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
                    e = Utils::expr_int(arg->loc, sym->type->size);
                }
                                
                else {                    
                    err("[ERROR]: could not resolve sizeof %s\n", sym->name);
                    exit(1);
                }

                if(!e){
                    err("[ERROR]: compiling sizeof got null expression.\n");
                    assert(0);
                }

                return compile_expr(e, State::CompilerState::None());
            }
    
            if(base->name == keyword_syscall){
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
                return type_int(64, true);
            }                                    
        }
        if(args.len() >= 0){
                
            Type* base_t = compile_expr(base, State::CompilerState::None());        
            bool isVa  = false;
            int  vaPos = 0;
            if(base_t->kind == TYPE_PROC){
                isVa  = base_t->proc.is_vararg;
                vaPos = base_t->proc.params.len() - 1;
            }            
                        
            if(!isVa){
                push("rax", base_t);
                assert(args.len() < MAX_ARGREG);
            }            
            int id = 0;                                                               
            for(Expr* arg: args) {                            
                if(isVa and (id >= vaPos)) break;                    
                id++;

                Type* t;
                if(arg->kind == EXPR_NAME){                        
                    if(Sym* sym = sym_get(arg->name)){
                        if(sym->type->kind == Ast::TYPE_STRUCT or sym->type->kind == Ast::TYPE_UNION){                            
                            t = compile_expr(arg, State::CompilerState::GetAddress());
                            push("rax", type_ptr(t, t->ismut));
                            continue;
                        }
                    }
                }
                t = compile_expr(arg, State::CompilerState::None());
                push("rax", type_ptr(t, t->ismut));                
            }                   
            
            
            

            int argPos = 0;
            int va_offset = 0;            
            for(Expr* arg: args){                
                (void) arg;

                if(isVa and (argPos >= vaPos)) {                        
                    for(int i = vaPos - 1; i < args.len(); i++){
                        Expr* va_arg = args.at(i);                        
                        compile_expr(va_arg, State::CompilerState::None());
                        println("mov qword [INTERNAL__va_stack + %i], rax", va_offset);
                        va_offset += sizeof(void*);
                    }                  
                    println("mov qword [INTERNAL__va_stack + %i], INTERNAL__va_end", va_offset);                                            
                    break;
                }

              
                Type* t = pop_nostack("rax");
                mov_to_rax(t, reg_from_size(t->size, --id));    
                argPos++;        
            }
            if(!isVa){
                pop("rax");            
            }
            else {                
                
                compile_expr(base, State::CompilerState::None());
            }           
            
            println("call rax");
            return base_t;
        }
        else {                                    
            Type* base_t = compile_expr(base, State::CompilerState::None());        
            if(base_t->kind != TYPE_PROC){
                err("Expected base_t to be procedure but got %s", base_t->repr());
                exit(1);
            }
            println("call rax");
            return base_t->proc.ret_type;
        }
        
    }
    Type* compile_name(const char* name, State::CompilerState& state){                        
        // Check for procedures
        
        if(Procedures::Procedure* cp = ctx.findProcedureByName(name)){            
            println("mov rax, %s", cp->name);            
            assert(cp->returnType);             
            return cp->returnType;
        }        
        
        // Check for globals        
        if(Variables::Variable* global = ctx.getGlobalVariables(name)){
            lea(global);
            if(state != State::CompilerState::GetAddress()){
                load(global->type);
            }
            return global->type;
        }

        // Check for parameters
        Procedures::Procedure* proc = ctx.getCurrentProcedure();
        if(proc){                    
            if(Variables::Variable* param = proc->findParameter(name)){
                makeLabel();
                lea(param);
                if(state != State::CompilerState::GetAddress()){
                    load(param->type);
                }
                return param->type;
            }
            if(Variables::Variable* local = proc->findLocalVariable(name)){                
                makeLabel();
                lea(local);          
                // TODO: Load if the state is not get_addr and if the local variable is not a non-pointer to a struct object                
                if(state != State::CompilerState::GetAddress()){                                    
                    load(local->type);                    
                }
                
                return local->type;
            }                  
        }   
               
        if(Constants::ConstantExpression* ce = ctx.getConstantExpression(name)){
            return compile_expr(ce->expr, state);
        }  
        // Check for types 
        if(TypeDefinitions::TypeDefinition* tf = typedefs.getTypeDefinition(name)){            
            return tf->type;            
        }
    
        err("[ERR]: name '%s' not found in this scope.\n", name);
        exit(1);
    }

    Type* compile_index_offset(Type* ty, State::CompilerState& state){
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

        if(state != State::CompilerState::GetAddress()){
            return load(ty->base);
        }
        else {
            return ty;
        }
    }
    Type* compile_index(Expr* base, Expr* index, State::CompilerState& state){
        Type* tb = compile_expr(base, State::CompilerState::None());
        if(!tb->base){
            err("[ERROR]: Tring to get the index of a non-pointer type.\n");
            exit(1);
        }
        push("rax", tb);

        Type* in = compile_expr(index, State::CompilerState::None());                
        int base_sz = tb->base->size;
        return compile_index_offset(tb, state);        
    }
    Type* compile_field(Expr* parent, Expr* children, State::CompilerState& state){    
        // Check if we are trying to get the field of an enumeration        
        if(Sym* sym = sym_get(parent->name)){
            if(sym->kind == Resolver::SYM_ENUM){                
                if(Sym* impl = sym->impls.find(children->name)){                    
                    Expr* e = impl->decl->expr;
                    assert(e->kind == EXPR_INT);
                    println("mov rax, %li", e->int_lit);                    
                    return type_int(64, impl->type->ismut);
                }
            }            
        }
        


        Type* p = compile_expr(parent, State::CompilerState::GetAddress());        
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
                if(Sym* impl = impls.find(children->name)){
                    println("mov rdi, rax");
                    //println("mov rax, rdi");
                    assert(impl->kind == SYM_PROC);
                    println("mov rax, %s", impl->name);
                    return impl->type->proc.ret_type;
                }
            }        
            if(p->kind == TYPE_STRUCT or p->kind == TYPE_UNION){
                bool isUnion = p->kind == TYPE_UNION;
                size_t offset = 0;
                for(TypeField* field: p->aggregate.items){
                    if(field->name != children->name){
                        if(!isUnion){
                            offset += field->type->size;
                        }                        
                        continue;
                    }

                    if(offset > 0){
                        // rax: lea 
                        println("add rax, %zu", offset);
                    }
                    if(state != State::CompilerState::GetAddress()){
                        load(p);
                    }
                    return field->type;
                }
            }            
            err("target = %s\n", p->name);
            err("[ERROR]: field %s is not existent in the object %s.", children->name, p->repr());
            exit(1);            
        }
        
        err("[ERR]: get field children must be <str>.\n");
        exit(1);                        
    }
    Type* compile_cast(TypeSpec* ts, Expr* expr, State::CompilerState& state){
        assert(ts->resolvedTy);
        Type* ty = ts->resolvedTy;
        compile_expr(expr, state);
        return ty;
    }
    Type* compile_ternary(Expr* cond, Expr* then, Expr* otherwise){        
        // rax -> 1, 0
        println(";; ----------------------------------\n");
        cmp_zero(compile_expr(cond, State::CompilerState::None()));
        int end_addr = count();
        int else_addr = count();        
        println("je .L%i", else_addr);
        Type* lhs = compile_expr(then, State::CompilerState::None());
        println("jmp .L%i\n", end_addr);
        makeLabel(else_addr);
        compile_expr(otherwise, State::CompilerState::None());
        makeLabel(end_addr);
        return lhs;   
    }
    Type* compile_expr_switch(Expr* e, State::CompilerState& state){
        assert(e->kind == EXPR_SWITCH);
        Expr* cond = e->expr_switch.cond;
        
        auto cases = e->expr_switch.cases;
        auto has_default = e->expr_switch.has_default;
        auto default_case = e->expr_switch.default_case;            

        Type* cond_t = compile_expr(cond, state);
        cmp_zero(cond_t);
        
        int end = count();

        for(SwitchCase* c: cases){
            // if __eq__(cond, case) {block, skip_end}
            int begin       = count();
            int end_case    = count();

            for(SwitchCasePattern* pt: c->patterns){
                if(pt->begin != pt->end){
                    Expr* lt = Utils::expr_binary(
                        {},
                        TK_less,
                        Utils::expr_int({}, pt->begin),
                        cond);

                    Expr* gt = Utils::expr_binary(
                        {},
                        TK_greater,
                        Utils::expr_int({},pt->begin),
                        cond);

                    Expr* lt_and_gt = Utils::expr_binary({}, TK_and, lt, gt);
                    Type* cond_t = compile_expr(lt_and_gt, state);
                    cmp_zero(cond_t);
                    println("jne .L%i", begin);
                }
                else {
                    err("IDK daDAS\n");
                    exit(1);
                }
            }
            println("jmp .L%i", end_case);
            makeLabel(begin);
            compile_block(c->block);
            makeLabel(end_case);
        }


        if(has_default){
            makeLabel(end);
            int after_end = count();
            println("jmp .L%i", after_end);
            compile_block(default_case);
            makeLabel(after_end);
        }

        else {
            makeLabel(end);
        }

        return cond_t;
    }    
    Type* compile_lambda(Expr*& e){                
        Operand f = resolve_lambda(e);
        Sym* lambda = Declared_lambdas.back();
        assert(lambda);
        post_declared_compile.push(lambda);
        
        SVec<Expr*> args;
            args.push(Utils::expr_name(e->loc, lambda->name));

        Expr* call = Utils::expr_call(
            e->loc,
            Utils::expr_name(e->loc, lambda->name),
            args
        );
        
        return compile_expr(Utils::expr_name(e->loc, lambda->name), State::CompilerState::None());
    }
    Type* compile_new(Expr*& e){
        if(!new_allocator){
            err("[ERROR]: `new` allocator not configured in the compiler, compilation will be aborted.\n");
            exit(1);
        }
        Sym* s = sym_get(new_allocator->name);
        assert(s);

        SVec<Expr*> &args = e->new_expr.args;
        Expr*       &size = e->new_expr.size;
        TypeSpec*   &type = e->new_expr.type;

        if(args.len() + 1 > MAX_ARGREG){
            printf("[ERROR]: new operator max args.\n");
            exit(1);
        }            

        Type* rty = type->resolvedTy;
        if(rty->size == 0){
            err("[ERROR]: INTERNAL rty->size == 0 for %s\n", rty->repr());
            exit(1);
        }

        /*
            Objective:
                rax -> new_proc(item_size, list_size)
            Memory allocation:
                item_nbytes -> rdi | argreg64[0]
                list_items  -> rsi | argreg64[1]                        
        */        
        SVec<Expr*> call_args;
            /* rdi -> */ call_args.push(Utils::expr_int(e->loc, rty->size));     // item nbytes
            /* rsi -> */ call_args.push(size);                               // list size
        
        Expr* new_call = Utils::expr_call(
            e->loc,
            Utils::expr_name(e->loc, s->name),
            call_args
        );
        Type* ptr_ty = compile_expr(new_call, State::CompilerState::None());
        if(args.len() == 0) return ptr_ty;
        if(type->kind != Ast::TYPESPEC_NAME){
            err("[ERROR]: Compiling typespec in `new` operator expects type name, got: %s.\n", type->resolvedTy->repr());
            exit(1);
        }
        int reg_id = 0;
        push(reg_from_size(ptr_ty->size, reg_id), ptr_ty);
        push(reg_from_size(ptr_ty->size, reg_id++), ptr_ty);
        

        Sym* structure = sym_get(type->name);
        Sym* method = structure->impls.find("constructor");
        if(!method){
            err("[ERROR]: trying to pass arguments in new operator for a struct with no constructor\n");
            exit(1);
        }
        assert(method->kind == Resolver::SYM_PROC);

        if(method->decl->proc.params.len() != args.len() + 1){
            int expected    = method->decl->proc.params.len();
            int got         = args.len();
            err("[ERROR]: Trying to call constructor of `%s` that expects %i arguments but just got %i\n", 
                ptr_ty->repr(),
                expected,
                got);
            exit(1);
        }
        
        
        // stack = ... ptr                
        for(Expr*& arg: args){
            /* rax -- nth-arg */Type* arg_ty = compile_expr(arg, State::CompilerState::None());
            push(rax_reg_from_size(arg_ty->size), arg_ty);
        }
        // stack = ... ptr ...args
        
        reg_id = 1;
        
        for(Expr*& arg: args){
            (void) arg;
            Type* ty = stack.back();
            pop(reg_from_size(ty->size, args.len() + 1 - reg_id++));
        }        
        pop("rax");
        println("mov %s, rax", reg_from_size(ptr_ty->size, 0));
        println(";; NEW WITH ARGS");
        println("call %s", method->name);
        pop(rax_reg_from_size(ptr_ty->size));
        /* rax -- pointer */ 
        return ptr_ty;
    }

    Type* compile_expr(Expr* e, State::CompilerState& state){                
        //static int i = 0;
        
        switch(e->kind){
            case EXPR_INT:                
                makeLabel();
                println("mov rax, %zu", e->int_lit);
                return type_int(64, true);
            case EXPR_STRING:   {
                makeLabel();
                int str_id = Factory::createGlobalString(global_strings, e->string_lit);
                println("mov rax, S%i", str_id);                
                return type_string(false);
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
                    State::CompilerState::None()
                );            
            case EXPR_SWITCH:       return compile_expr_switch(e, state);
            case EXPR_CALL:         return compile_call(e->call.base, e->call.args, state);
            case EXPR_CAST:         return compile_cast(e->cast.typespec, e->cast.expr, state);                
            case EXPR_ARRAY_INDEX:  return compile_index(e->array.base, e->array.index, state);
            case EXPR_FIELD:        return compile_field(e->field_acc.parent, e->field_acc.children, state);
            case EXPR_TERNARY:      return compile_ternary(e->ternary.cond, e->ternary.if_case, e->ternary.else_case);
            case EXPR_LAMBDA:       return compile_lambda(e);
            case EXPR_NEW:          return compile_new(e);
            default:                 
                fprintf(stdout, "========[ ERROR PLANG UNKNOWN EXPRESSION COMPILATION BRANCH ]================\n");
                fprintf(stdout, "Got expression: ");
                pPrint::expr(e);
                fprintf(stdout, "\n");
                fprintf(stdout, "=============================================================================\n");
                exit(1);
        }
    }
    Type* compile_is_between(Expr* cond, int begin, int end){        
        println("; compile_is_between");
        Expr* low       = Utils::expr_binary(cond->loc, TK_less, cond, Utils::expr_int(cond->loc, begin));
        Expr* high      = Utils::expr_binary(cond->loc, TK_greater, cond, Utils::expr_int(cond->loc, end));
        Expr* logic     = Utils::expr_binary(cond->loc, TK_or, low, high);
        return compile_expr(logic, State::CompilerState::None());        
    }
    void compile_switch_case(Stmt* s){        
        if(DEBUG_MODE){
            static bool warned = false;
            if(!warned){
                err("[WARN]: switch case is in development process.\n");        
                warned = true;
            }
        }
        assert(s->kind == Ast::STMT_SWITCH);        
        int end_addr    = count();
        int case_begin_addr;
        int case_end_addr;
        
        for(SwitchCase* c: s->stmt_switch.cases){
            case_begin_addr = count();
            case_end_addr   = count();
            SVec<SwitchCasePattern*> patterns = c->patterns;
            for(auto* p: patterns){
                if(p->begin != p->end){                    
                    if(p->begin < p->end){
                        Type* ty = compile_is_between(s->stmt_switch.cond, p->begin, p->end);
                        cmp_zero(ty);                                                                        
                    }
                    else {
                        compile_expr(s->stmt_switch.cond, State::CompilerState::None());
                        println("cmp rax, %i\n", p->begin);
                    }
                    
                    if(p == patterns.back()){
                        println("jne .L%i", case_end_addr);
                    }
                    else {
                        println("je .L%i", case_begin_addr);
                    }
                }
                
                else {
                    if(p->name){                    
                        Sym* sym = sym_get(p->name);
                        if(!sym){
                            err("%s is not found.\n", p->name);
                            exit(1);
                        }

                        if(sym->kind == SYM_ENUM){
                            Decl* d_enum = sym->decl;
                            if(d_enum->kind == Ast::DECL_CONSTEXPR){
                                Expr* e = d_enum->expr;
                                if(e->kind == EXPR_INT){
                                    int int_val = e->int_lit;                                
                                    compile_expr(s->stmt_switch.cond, State::CompilerState::None());
                                    println("cmp rax, %i\n", int_val);
                                    println("jne .L%i", case_end_addr);
                                    continue;
                                }
                                else if(e->kind == EXPR_STRING){
                                    if(!string_comparator){
                                        err("[WARN]: Didn't found any string comparator procedure defined with @string_comparator.\n");
                                        exit(1);
                                    }

                                    Sym* scmp = sym_get(string_comparator->name);
                                    assert(scmp);
                                    assert(scmp->kind == Resolver::SYM_PROC);
                                    assert(scmp->decl->proc.params.len() == 2);
                                    assert(scmp->decl->proc.is_complete);

                                    SVec<Expr*> args;
                                    args.push(s->stmt_switch.cond);
                                    args.push(Utils::expr_string(e->loc, e->string_lit));
                                    Expr* str_cmp_expr = Utils::expr_call(
                                        e->loc,
                                        Utils::expr_name(e->loc, scmp->decl->name),
                                        args
                                    );

                                    compile_expr(str_cmp_expr, State::CompilerState::None());                                    
                                    // Expected @stack [... bool]
                                    println("cmp rax, 0");
                                    println("jne .L%i", case_end_addr);
                                    continue;
                                }
                            }

                            // Else falltrough                        
                        }
                    }
                    
                    else {
                        if(not p->string){                            
                            compile_expr(s->stmt_switch.cond, State::CompilerState::None());
                            println("cmp rax, 0\n");
                            println("jne .L%i", case_end_addr);
                            continue;
                        }
                        if(!string_comparator){
                            err("[WARN]: Didn't found any string comparator procedure defined with @string_comparator.\n");
                            err("[NOTE]: Maybe you should use stdlib, try: `use std{std}`.\n");
                            exit(1);
                        }
                        Sym* scmp = sym_get(string_comparator->name);
                        assert(scmp);
                        assert(scmp->kind == Resolver::SYM_PROC);
                        assert(scmp->decl->proc.params.len() == 2);
                        assert(scmp->decl->proc.is_complete);

                        SVec<Expr*> args;
                        args.push(s->stmt_switch.cond);
                        args.push(Utils::expr_string({}, p->string));
                        Expr* str_cmp_expr = Utils::expr_call(
                            {},
                            Utils::expr_name({}, scmp->decl->name),
                            args
                        );

                        compile_expr(str_cmp_expr, State::CompilerState::None());                        
                        // Expected @stack [... bool]
                        println("cmp rax, 0");
                        println("je .L%i", case_end_addr);
                        continue;
                    }                    
                    assert(p->name);
                    Sym* sym = sym_get(p->name);                        
                    if(Constants::ConstantExpression* ce = ctx.getConstantExpression(sym->name)){
                        if(ce->expr->kind == EXPR_INT){
                            compile_expr(s->stmt_switch.cond, State::CompilerState::None());
                            println("cmp rax, %li\n", ce->expr->int_lit);
                            println("jne .L%i", case_end_addr);                            
                            goto compile_body;
                        }
                    }
                    else
                    {                    
                        err("Cant compile parrern name yet %s.\n", p->name);
                        exit(1);                    
                    }
                }                
            }

            compile_body:
            makeLabel(case_begin_addr);
            compile_block(c->block);
            println("jmp .L%i", end_addr);
            makeLabel(case_end_addr);
        }
        if(s->stmt_switch.has_default){     
            // falltrough      
            compile_block(s->stmt_switch.default_case);
        }
        makeLabel(end_addr);
    }
    void compile_stmt(Stmt* stmt){
        switch(stmt->kind){
            case Ast::STMT_EXPR:    {
                compile_expr(stmt->expr, State::CompilerState::None());                
            } break;
            case Ast::STMT_FOR: {                
                assert(0 && "unimplemented");
            };
            case Ast::STMT_IF:  {                
                int if_end   = count();
                {
                    Type* cond = compile_expr(stmt->stmt_if.if_clause->cond, State::CompilerState::None());
                    cmp_zero(cond);
                    println("je .L%i", if_end);
                    compile_block(stmt->stmt_if.if_clause->block);
                    
                }
                if(stmt->stmt_if.elif_clauses.len() > 0){
                    makeLabel(if_end);
                    if_end = count();
                    for(IfClause* elif: stmt->stmt_if.elif_clauses){
                        int elif_end = count();                        
                        
                        Type* cond = compile_expr(elif->cond, State::CompilerState::None());
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
                if(stmt->stmt_while->is_doWhile){
                    compile_block(stmt->stmt_while->block);
                    Type* ty = compile_expr(stmt->stmt_while->cond, State::CompilerState::None());
                    cmp_zero(ty);
                    println("je .L%i", end);
                    println("jmp .L%i", begin);                    
                }
                else {                    
                    
                    Type* ty = compile_expr(stmt->stmt_while->cond, State::CompilerState::None());
                    cmp_zero(ty);
                    println("je .L%i", end);
                    compile_block(stmt->stmt_while->block);
                    println("jmp .L%i", begin);                    
                }
                makeLabel(end);
            } break;
            case Ast::STMT_RETURN:  {
                Procedures::Procedure* proc = ctx.getCurrentProcedure();
                assert(proc);
                compile_expr(stmt->expr, State::CompilerState::None());
                println("jmp .PE.%s", proc->name);
              
            } break;
            case Ast::STMT_SWITCH:
                compile_switch_case(stmt);
                break;
            default: assert(0);
        }
    }
    void compile_block(SVec<Stmt*> block){
        for(Stmt* stmt: block) 
            compile_stmt(stmt);
    }
    
    void compile_variable_deleter(Variables::Variable* var, Sym* del){
        if(var->type->kind == TYPE_PTR) return;
        err("deleter of %s\n", var->name);
        Expr* e = Utils::expr_field_access(
            {},
            Utils::expr_name({}, var->name), 
            Utils::expr_name({}, AGGREGATE_DELETER_WORD));
                        
        compile_expr(e, State::CompilerState::None());
        println("call rax");
    }
    void compile_decl_proc(Decl* d){                
        if(d->proc.is_internal) return;
        
        for(Note* note: d->notes){
            if(note->name == keyword_warn){
                for(Expr* arg: note->args){
                    assert(arg->kind == Ast::EXPR_STRING);
                    err("[WARN]: %s\n", arg->string_lit);
                }
            }
            else if(note->name == keyword_extern) {
                has_extern = true;
                if(note->args.len() != 1){
                    printf("[ERROR]: extern expects the path to the .asm of the extern file.\n");
                    exit(1);
                }
                assert(note->args.at(0)->kind == Ast::EXPR_STRING);

                extern_paths = strf("%s %s", extern_paths, note->args.at(0)->string_lit);
                return;
            }
        }

        Procedures::Procedure* proc = ctx.findProcedureByName(d->name);                
        ctx.setCurrentProcedure(proc);
        if(!proc){
            err("Got no proc in proc %s\n", d->name);
            exit(1);
        }
        

        printf("%s:\n", proc->name);
        printlb(".PB.%s", proc->name);
        println("push rbp");
        println("mov rbp, rsp");        

        if(proc->stackAllocation > 0){
            println("sub rsp, %zu", proc->stackAllocation);
        }

        assert(proc->parameters->len() < MAX_ARGREG);
        {
            int id = 0;
            for(Variables::Variable* param: *proc->parameters){
                // Force struct objects to be pointer
                if(param->type->kind != TYPE_PTR and (param->type->kind == TYPE_STRUCT or param->type->kind == Ast::TYPE_UNION)){
                    param->type = type_ptr(param->type, param->type->ismut);
                }
                println("mov [rbp - %i], %s", param->stackOffset, argreg64[id++]);
            }
        }
        compile_block(d->proc.block);        
        println("mov rax, 0");
        printlb(".PE.%s", proc->name);
        Procedures::Procedure* p = ctx.findProcedureByName(d->name);
        assert(p);        
        for(Variables::Variable* local: *p->localVariables){            
            if(local->type->kind != TYPE_PTR){                
                Sym* local_type_sym = sym_get(local->type->name);
                if(local_type_sym){
                    SymImpl& impls = local_type_sym->impls;            
                    if(Sym* del = impls.find(AGGREGATE_DELETER_WORD)){                                                                 
                        Type* t = local_type_sym->type;
                        assert(t->kind == TYPE_STRUCT);

                        int stack_offset = 0;
                        if(0){
                            for(TypeField* tf: t->aggregate.items){
                                Sym* child_sym = sym_get(tf->type->name);
                                stack_offset += tf->type->size;

                                if(Sym* child_killer = child_sym->impls.find(AGGREGATE_DELETER_WORD)){
                                    if(stack_offset > 0){
                                        lea(local);
                                        println("add rax, %i", stack_offset)
                                        load(local, State::CompilerState::None());
                                        // println("mov rax, [rax]")
                                    }
                                    else {
                                        //lea(local);
                                        mov(local);
                                    }

                                    // Dummy allocation
                                    Variables::Variable* child_var = new Variables::Variable {
                                        child_sym->name,
                                        child_sym->type,
                                        nullptr,
                                        false,
                                        false,
                                        stack_offset                                    
                                    };
                                    compile_variable_deleter(child_var, child_killer);
                                }                            
                            }
                        }
                        // Delete the parent struct
                        compile_variable_deleter(local, del);                                        
                    }                
                }
            }
        }        
        println("leave");
        println("ret");                
        ctx.clearCurrentProcedure();
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
        if(global_strings.len() > 0){
            printf("segment .data\n");
            int id = 0;
            for(std::string str: global_strings){
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
        if(buffer_size > 0 or ctx.globalVariables.len() > 0){                        
            for(Variables::Variable* global: ctx.globalVariables){
                    printc("G%s: resb %i\n", global->name, global->type->calcSize());
            }
        }

        println("INTERNAL__va_at:       resq 1");        
        println("INTERNAL__va_begin:    resq 0");        
        println("INTERNAL__va_stack:    resq %i", VARIADIC_ARGS_CAP);        
        println("INTERNAL__va_end:      resq 1");
        
        
    }
    
    void compile_ast(SVec<Decl*> ast, COMPILER_TARGET target){        
        
        P_SET_CTXOUT("pietra.asm")
        

        printf("BITS 64\n");
        printf("segment .text\n");
        println("global _start:");

    
                
        for(Decl* decl: ast){            
            compile_decl(decl);
        }
        for(Sym* post: post_declared_compile){
            Decl* decl = post->decl;
            assert(decl);            
            compile_decl(decl);
        }

        printlb("_start");
        Procedures::Procedure* pmain = ctx.findProcedureByName(keyword_main);
        if(!pmain){
            err("[ERROR]: Expected the 'main' procedure to be declared\n");
            exit(1);
        }        
        // Initialize allocator __heap_end__
        println("mov rax, 12");
        println("mov rdi, 0");
        println("syscall");        
        println("mov [__heap_end__], rax")
        for(Variables::Variable* global: ctx.globalVariables){
            assert(global->isGlobal);
            if(global->initializer){
                lea(global);
                push("rax", global->type);
                compile_expr(global->initializer, State::CompilerState::None());
                store();
            }

            
            const char* name = global->type->kind == TYPE_PTR? global->type->base->name: global->type->name;
            Sym* sym = sym_get(name);

            if(sym and sym->type->kind != TYPE_PTR and sym->kind == Resolver::SYM_AGGREGATE and sym->type->name){                                
                if(Sym* constructor = sym->impls.find(AGGREGATE_CONSTRUCTOR_WORD)){                                            
                    compile_aggregate_constructor(global, constructor);
                }                
            }            
        }
        if(pmain->parameters->len() != 0)
        {
            assert(pmain->parameters->len() == 2);
            println("mov %s, [rsp]", argreg64[0]);
            println("mov %s, rsp", argreg64[1]);
            println("add %s, 8\n", argreg64[1]);
            
        }
        println("call main");
        println("mov rdi, rax");
        println("mov rax, 60");        
        println("syscall");
        compile_segment_data();
        compile_segment_bss();                
        fclose(ctx.outputChannel);
        
        if(DEBUG_MODE){            
            err("[NOTE]: Compilation succesfuly.\n");            
        }
        else {
            switch(target){
                default:
                case CT_LINUX:
                {
                    system(strf("nasm -felf64 pietra.asm"));                        
                    system(strf("ld pietra.o -o pietra.bin"));
                    system(strf("rm pietra.o"));
                    break;                    
                }
                case CT_WINDOWS:
                {
                    printf("[WARN]: compiler target as windows hasn't been deployed yet, proceed with caution.\n");
                    system(strf("nasm -fwin64 pietra.asm -nostartfiles"));
                    system(strf("gcc pietra.obj -o pietra.exe -lkernel32 -luser32"));
                    system(strf("del pietra.obj"));
                    break;
                }
            }            
        }        
    }    
}
#undef printfz
#endif /*ASMPLANG*/
