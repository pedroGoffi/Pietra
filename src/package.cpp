#ifndef PACKAGE_CPP
#define PACKAGE_CPP
#include "../include/packages.hpp"
#include "pprint.cpp"
using namespace Pietra;

PPackage* PPackage::from(const char *filename){
    PPackage* package = arena_alloc<PPackage>();    
    package->cursor = PCursor::from(filename);
    if(!package->cursor) return nullptr;

    
    // Initialize the unresolved AST
    
    while((package->cursor->decl = Parser::decl())){
        package->ast.push(package->cursor->decl);        
    }
            
    return package;

}

SVec<Decl*> PPackage::get_all(){ return this->ast; }
Decl* PPackage::get_sym(const char* name){
    name = Core::cstr(name); // Intern the string 
    for(Decl* node: this->ast){
        if(node->name == name) return node;
    }
    return nullptr;
}
#endif /*PACKAGE_CPP*/