#ifndef ASMPLANG_HPP
#define ASMPLANG_HPP
#include "arena.hpp"
#include "interns.hpp"
#include "resolve.hpp"
#include "type.hpp"
#include <map>
#include "../src/interns.cpp"

using namespace Pietra;
using namespace Pietra::Resolver;
using PType = Ast::Type;

namespace Pietra::Asm {
    struct Compiled_var {
        const char* name;
        Ast::Type*  type;
        Expr*       init;
        int         stack_offset;
        bool        is_global;
    };

    enum Compiled_context{
        CTX_NONE,
        CTX_GET_ADDR,
        CTX_SYSCALL
    };    

    struct Compiled_proc{
        const char*             name;        
        bool                    is_recursive;
        size_t                  stack_allocation;        
        SVec<Compiled_var*>     params;
        SVec<Compiled_var*>     locals;
        

        // Returns the var stack position
        void allocate_local_var(Compiled_var*& var){
            assert(!var->is_global);
            this->stack_allocation += var->type->size;
            this->locals.push(var);
            var->stack_offset = this->stack_allocation;
            fprintf(stderr, "CP allocating, SOFF: %s at %zu\n", 
                var->name,
                this->stack_allocation);            
        }
    };
    struct Compiled_procs{    
        Compiled_proc* current_proc;    
        std::map<const char*, Compiled_proc*> procs;


        Compiled_proc* find(const char* name){
            name = Core::cstr(name);
            return (procs.find(name) != procs.end())
            ?   procs[name]
            :   nullptr;
        }
        
        void push(Compiled_proc* proc){
            assert(!this->find(proc->name));
            // Assert is internal string
            proc->name = Core::cstr(proc->name);
            procs[proc->name]   = proc;
            this->current_proc  = proc;
        }

        void stack_alloc(int size){
            assert(this->current_proc);
            this->current_proc->stack_allocation += size; 
        }

        void cp_recursive(bool state){
            assert(this->current_proc);
            this->current_proc->is_recursive = state;
        }

        Compiled_proc* cp_new(const char* name){
            assert(!this->find(name));
            Compiled_proc* cp = arena_alloc<Compiled_proc>();
            cp->name = Core::cstr(name);
            cp->stack_allocation    = 0;
            cp->is_recursive        = false;                        
            this->push(cp);
            return cp;
        }
    };


    struct Compiled_strings{
        SVec<const char*> strings;

        int push(const char* str){
            str = Core::cstr(str);
            strings.push(str);
            return strings.len();
        }
    };


    struct Compiled_vars {
        SVec<Compiled_var*> vars;


        Compiled_var* find(const char* name){
            name = Core::cstr(strf("__%s"));
            
            for(Compiled_var* v: vars){                
                if(name == v->name) return v;
            }            
            return nullptr;
        }

        void push(Compiled_var* v){
            assert(!find(v->name));
            vars.push(v);
        }

        Compiled_var* cv_new(const char* name, Ast::Type* type, Expr* init, bool is_global){
            Compiled_var* cv = arena_alloc<Compiled_var>();
            cv->name = Core::cstr(strf("__%s", name));
            cv->stack_offset = 0;
            cv->type = type;
            cv->init = init;            
            cv->is_global = is_global;
            this->push(cv);
            return cv;
        }
    };


    void compile_init_var(const char* name, TypeSpec* type, Expr* init, bool is_global);
    void opt_load();
    PType* compile_expr(Expr* expr);
        PType* compile_lea(Compiled_var* local);
        PType* compile_binary(Lexer::tokenKind kind, Ast::Expr* lhs, Ast::Expr* rhs);
        PType* compile_modify_var(Asm::Compiled_var* var, Ast::Expr* expr);
        void compile_cmp_zero(PType* type);
        PType* compile_logic_expr(Lexer::tokenKind kind, Expr* lhs, Expr* rhs);
        PType* compile_assign_var_expr(Expr* var_expr, Expr* rhs);
        void compile_pre_call_args(SVec<Expr*> args);
        
    void compile_stmt(Stmt* stmt);    
    void compile_stmt_block(SVec<Stmt*> block);
    void compile_decl(Decl* decl);        
        void compile_decl_proc(Decl* decl);
            void compile_proc_params(Compiled_proc* cp);
        
    void compile_ast(SVec<Decl*> ast);
    void compile_segment_data();
    void compile_segment_bss();

}



#endif /*ASMPLANG_HPP*/