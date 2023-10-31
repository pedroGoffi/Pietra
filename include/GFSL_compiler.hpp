#ifndef GFSL_compiler
#define GFSL_compiler
#include "../../GFSL2/compiler.cpp"
#include "Asmx86_64.hpp"
#include "arena.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include "smallVec.hpp"
#include <cstddef>

/*
    Note: This is the last Layer before GFSL structs | Instructions
*/
namespace Pietra::GFSLCompiler {
    using namespace Pietra::Ast;
    struct CVar{
        std::string     name;
        Type*           type;        
        bool            isGlobal;
        Expr*           init;        
        int             gfid;
        GFSL::Variable* gfvar;        
    };

    struct CProc{
        std::string name;
        SVec<CVar*> params;
        SVec<CVar*> locals;
        Type*       ret;
        
        int         gfid;
        GFSL::Proc* gfproc;
        int         allocation;
        
        CVar* find_param(std::string name){
            for(CVar* var: this->params){
                if(var->name == name) 
                    return var;
            }
            return nullptr;
        }
        void install_param(CVar* param){
            assert(!find_param(param->name));
            assert(param->gfvar);
            this->gfproc->install_param(*param->gfvar);

            this->params.push(param);
            
            this->allocation += param->type->size;
            param->gfvar->stack_offset = this->allocation;
            
            
        }

        CVar* find_local(std::string name){
            for(CVar* var: this->locals){
                if(var->name == name) 
                    return var;
            }
            return nullptr;
        }
        void install_local(CVar* local){
            assert(!find_param(local->name));
            assert(!find_local(local->name));
            
            assert(local->gfvar);            
            local->gfid = this->gfproc->local_vars.size();
            this->gfproc->install_var(*local->gfvar);            
            this->locals.push(local);            

            
            this->allocation += local->type->size;
            local->gfvar->stack_offset = this->allocation;
            
            
        }

        static CProc* Init(std::string name){
            CProc* cp = arena_alloc<CProc>();
            cp->name = name;
            cp->gfproc = GFSL::make_proc(
                GFSL::program_size(), 
                name, 
                {}, 
                {},
                false);

            GFSL::ctx.cp = cp->gfproc;
            return cp;
        }
    };

    enum CState {
        C_NONE,
        C_GET_ADDR,        
    };
    struct CContext{
        CState          state;
        SVec<CProc*>    procs;
        SVec<CVar*>     globals;

        CProc*          cp;
        bool expect(CState state){
            if(this->is(state)){
                this->reset();
                return true;
            }
            return false;
        }
        bool is(CState state){
            return this->state == state;
        }
        void set(CState state){
            this->state = state;
        }
        void reset(){
            this->state = C_NONE;
        }
        // returns the stream index
        int install_str(std::string str){            
            return GFSL::ctx.install_str(str);
        }

        CVar* find_var(std::string str){
            for(CVar* var: this->globals){
                if(var->name == str){
                    return var;
                }
            }
            return nullptr;
        }
        void install_global(CVar* var){
            assert(!find_var(var->name));
            assert(var->gfvar);


            this->globals.push(var);
            var->gfid = GFSL::ctx.globals.size();
            GFSL::ctx.install_global(*var->gfvar);
        }

        CProc* find_proc(std::string name){
            for(CProc* proc: this->procs){
                if(proc->name == name) return proc;
            }
            return nullptr;
        }

        void install_proc(CProc* proc){
            assert(proc->gfproc);
            assert(!find_proc(proc->name));
            this->cp = proc;
            this->procs.push(proc);
            proc->gfid = GFSL::ctx.procs.size();
            GFSL::ctx.install_proc(proc->gfproc);
        }
        

    };


    Type* compile_init_var(std::string name, Type *type, Expr *init, bool is_global);
    Type* compile_name(const char* name);
    Type* compile_call(Expr* base, SVec<Expr*> args);
    Type* compile_unary(Lexer::tokenKind kind, Expr* e);
    Type* compile_binary(Lexer::tokenKind kind, Expr* left, Expr* right);
    Type* compile_expr(Expr* e);
    void compile_stmt(Stmt* stmt);
    void compile_block(SVec<Stmt*> block);
    void compile_cproc_params(CProc* cp, SVec<ProcParam*> params);
    void compile_decl_proc(Decl* d);
    void compile_decl(Decl* decl);

}
#endif /*GFSL_compiler*/