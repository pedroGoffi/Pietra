#ifndef CPP_BRIDGE
#define CPP_BRIDGE
#include "../include/bridge.hpp"
#include <cstring>


namespace Pietra::CBridge{
    X86Context          ctx;
    SVec<const char*>   DefinedNames;
    SVec<CConstexpr*>   constexprs;
    CState state_none       = {.kind=C_NONE};
    CState state_getaddr    = {.kind=C_GET_ADDR};    
    CState state_got_proc   = {.kind=C_GOT_PROC};
    
    
    bool checkNameRedefinition(const char* str){
        for(const char* name: DefinedNames){
            if(name == str) 
                return true;
        }
        return false;
    }
    void raiseException(const char* description){
        printf("[ERR]: %s\n", description);
        exit(1);        
    }    
    size_t CProc::calcStackAllocation(){
        size_t alloc = 0;
        
        for(CVar* var: *this->locals) alloc += var->type->size;
        for(CVar* var: *this->params) alloc += var->type->size;
        printf("calc sa = %zu\n", alloc);
        return alloc;
        
    }

    CVar* CProc::init_tmp_var(const char* name, Type* type){        
        if(this->tmp_vars_offset == 0){
            this->tmp_vars_offset = calcStackAllocation();
        }
        

        CVar* var = arena_alloc<CVar>();    
        var->name = name;        
        var->type = type;        
        var->stackOffset = this->tmp_vars_offset;        
        this->tmp_vars_offset += var->type->calcSize();        
        var->isGlobal = false;        
        return var;
    }
    
    CVar* CProc::get_tmp_var(const char* name){
        for(CVar* var: this->tmp_vars){
            if(var){                
                if(var->name == name){
                    return var;                
                }
            }
        }
        return nullptr;
    }

    void CProc::push_tmp_var(CVar* var){
        assert(ctx.Cp);
        assert(!this->get_tmp_var(var->name));
        this->tmp_vars.push(var);
    }
    void CProc::forget_tmp_vars(){
        this->tmp_vars.reset();
        this->tmp_vars_offset = 0;
    }
    

    CVar* CProc::find_var_from(const char* str, SVec<CVar*> *list) {
        assert(list);
        for(CVar* var: *list){
            if(var->name == str) 
                return var;
        }
        return nullptr;
    }
    CVar* CProc::find_local(const char* name){
        return this->find_var_from(name, this->locals);
    }
    CVar* CProc::find_param(const char* name){
        return this->find_var_from(name, this->params);
    }

    bool CProc::checkNameColision(const char* name){
        return  checkNameRedefinition(name)     or 
                this->find_local(name) != nullptr   or
                this->find_param(name) != nullptr
                ;
    }
    void CProc::allocateLocalVar(CVar* &var, bool isParam){
        if(checkNameColision(var->name)){
            raiseException(strf("local var with name colision: %s\n", var->name));            
        }
        
        this->stackAllocation += var->type->calcSize();
        
        var->stackOffset = this->stackAllocation;
        
        if(isParam){
            this->params->push(var);
        }
        else {
            this->locals->push(var);
        }
                
    }
    
        
    
    // NOTE: it returns the String location in the array
    SVec<const char*> Strs;
    int CreateGlobalString(const char* str){
        int id = Strs.len();
        Strs.push(str);        
        return id;
    }
    CProc* GetProc(const char* name){                
        for(CProc* proc: ctx.Cps){            
            if(proc->name == name) {
                ctx.Cp = proc;
                return proc;
            }
        }
        return nullptr;
    }
    CProc* CreateProc(const char* name, Type* type){                        
        if(checkNameRedefinition(name)){
            raiseException(strf("name redeclaration of procedure %s", name));            
        }
        CProc* cp = arena_alloc<CProc>();
        cp->name = Core::cstr(name);            
        cp->type = type;
        cp->locals = arena_alloc<SVec<CVar*>>();
        cp->params = arena_alloc<SVec<CVar*>>();

        ctx.Cp = cp;
        DefinedNames.push(cp->name);
        ctx.Cps.push(cp);
        return cp;            
    }
    SVec<CVar*> globals;
    CVar* find_global(const char* name){
        for(CVar* var: globals){
            if(var->name == name) return var;
        }
        return nullptr;
    }
    void push_global(CVar* var){
        assert(!find_global(var->name));
        globals.push(var);
    }

    CVar* CreateVar(const char* name, Type* type, Expr* init, bool isGlobal = false, bool isParam = false){        
        if(checkNameRedefinition(name)){
            raiseException(strf("name redeclaration of variable %s", name));
        }
        CVar* var = arena_alloc<CVar>();
        var->name = Core::cstr(name);
        var->name = name;
        var->type = type;
        var->init = init;
        var->isGlobal = isGlobal;
        

        if(isGlobal){
            push_global(var);
        }
        else {
            assert(ctx.Cp);
            ctx.Cp->allocateLocalVar(var, isParam);
        }
        
        return var;
    }    
    int* get_enum(const char* str){
        if(ctx.enumMap.find(str) != ctx.enumMap.end()){
            return &ctx.enumMap[str];
        }

        return nullptr;
    }
    void set_enum(const char* str, int value){
        if(get_enum(str)){
            printf("[ERROR]: constexpr redeclaraton %s.\n", str);
            exit(1);
        }

        ctx.enumMap[str] = value;
    }
    
    

    Type*   tmp_self = nullptr;
    void set_self_t(Type* t){
        tmp_self = t;
    }
    void reset_self_t(){
        tmp_self = nullptr;
    }
    std::map<const char*, Type*> aliasM;
    Type* type_get(const char* name){
        name = Core::cstr(name);
        if(aliasM.find(name) != aliasM.end()){
            return aliasM[name];
        }

        return nullptr;
    }
    void type_set(const char* name, Type* type){
        name = Core::cstr(name);
        assert(!type_get(name));
        aliasM[name] = type;
    }
    CConstexpr* find_constexpr(const char* name){
        for(auto& ce: constexprs){
            if(ce->name == name) return ce;
        }
        return nullptr;
    }
    void save_constexpr(const char* name, Expr* expr){
        assert(!find_constexpr(name));
        assert(!checkNameRedefinition(name));

        CConstexpr* ce = CConstexpr::New(name, expr);
        constexprs.push(ce);
        DefinedNames.push(name);
    }
}
#endif /*CPP_BRIDGE*/