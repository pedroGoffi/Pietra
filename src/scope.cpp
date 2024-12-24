#ifndef PIETRA_SCOPE_CPP
#define PIETRA_SCOPE_CPP
#include "../include/scope.hpp"
#include "../include/resolve.hpp"
#include <vector>

namespace Pietra{
void Scope::add_sym(Resolver::Sym* sym) {        
    m_symbols.push_back(sym);
}


int Scope::scope_level(){
    return this->m_lvl;
}


Resolver::Sym* Scope::find_sym(const char* name) {    
    for (auto sym : m_symbols) {
        if (sym->name == name) {            
            return sym;
        }
    }
    return nullptr;
}

int ScopeManager::get_scope_level(){
    return this->m_lvl;
}
void ScopeManager::enter_scope() {
    m_scopes.push_back(new Scope);
    this->m_scopes.back()->m_lvl = this->m_lvl++;
}

void ScopeManager::leave_scope() {
    if (!m_scopes.empty()) {
        m_scopes.pop_back();
    }
}
int ScopeManager::add_sym(Resolver::Sym* sym) {
    if (!m_scopes.empty()) {
        this->m_scopes.back()->add_sym(sym);
        return 0;
    }
    return -1; // No scope to add symbol to
}
Resolver::Sym* ScopeManager::find_sym(const char* name) {
    
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {                
        Scope* scope = *it;
        assert(scope);
        if (Resolver::Sym* sym = scope->find_sym(name)) {    
            return sym;
        }
    }
    return nullptr;
}

}
#endif /*PIETRA_SCOPE_CPP*/