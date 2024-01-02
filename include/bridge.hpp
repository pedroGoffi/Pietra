#ifndef BRIGGE
#define BRIGGE
#include "arena.hpp"
#include "smallVec.hpp"
#include "type.hpp"
#include "interns.hpp"
#include <map>

using namespace Pietra::Core;
using namespace Pietra::Ast;
/*
Bridge idea is to make the AST code to lower level making it easier to compile to llvm or assembly x86_64
*/
namespace Pietra::CBridge {    
    bool checkNameRedefinition(const char* str);
    void raiseException(const char* description);

    struct CConstexpr{
        const char* name;
        // TODO: constexpr arguments
        Expr*       expr;

        static CConstexpr* New(const char* name, Expr* expr){
            CConstexpr* ce = arena_alloc<CConstexpr>();            
            ce->name = name;
            ce->expr = expr;
            return ce;
        }
    };

    struct CVar {      
      const char *name;
      Type *type;
      Expr *init;
      bool isGlobal = false;
      int stackOffset;              
    };


    struct CProc{
        const char*     name;        
        bool            isRecursive;
        size_t          stackAllocation;        
        Type*           type;
        SVec<CVar*>     *params = arena_alloc<SVec<CVar*>>();
        SVec<CVar*>     *locals = arena_alloc<SVec<CVar*>>();

        int             tmp_vars_offset;
        SVec<CVar*>     tmp_vars;


        size_t calcStackAllocation();
        CVar* get_tmp_var(const char* name);
        void push_tmp_var(CVar* var);
        void forget_tmp_vars();
        CVar* init_tmp_var(const char* name, Type* type);
        CVar* find_var_from(const char* str, SVec<CVar*> *list);
        CVar* find_local(const char* name);
        CVar* find_param(const char* name);
        bool checkNameColision(const char* name);
        void allocateLocalVar(CVar* &var, bool isParam);
    };
        
    enum CStateKind {
        C_NONE,
        C_GET_ADDR,          
        C_GOT_PROC
    };
    struct CState{        
        CStateKind kind;

        CState& operator=(CState& other){
            this->set(other.kind);
            return *this;
        }
        bool operator==(CState& other){
            return *this == other.kind;
        }
        bool operator!=(CState& other){
            return *this != other.kind;
        }        

        bool operator==(CStateKind other_kind){
            return this->kind == other_kind;
        }
        bool operator!=(CStateKind other_kind){
            return this->kind != other_kind;
        }        
        

        bool expect(CStateKind k){
            if(this->is(k)){
                this->set(C_NONE);
                return true;
            }
            return false;
        }
        void set(CStateKind k){
            this->kind = k;
        }
        bool is(CStateKind k){
            return this->kind == k;
        }
    };    

    struct X86Context {
        CState              state;          // State of the compiler
        FILE*               OUT;            // Output channel
        SVec<CProc*>        Cps;            // Compiled procs
        CProc*              Cp;             // Current proc
        SVec<std::string>   Strs;           // Strings
        CVar                GVars;          // Global vars        

        std::map<const char*, int> enumMap;        
        CState   ACtx;   // Assembly context

        
    };
    
    int CreateGlobalString(std::string str);
    int CreateGlobalString(const char* str);
    CVar* CreateVar(const char* name, Type* type, Expr* init, bool isGlobal, bool isParam);
    int* get_enum(const char* str);
    void set_enum(const char* str, int val);
    CProc* GetProc(const char* name);
    CProc* CreateProc(const char* name, Type* type);    
}
#endif /*BRIGGE*/