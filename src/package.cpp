#ifndef PACKAGE_CPP
#define PACKAGE_CPP
#include "../include/packages.hpp"
#include "pprint.cpp"
using namespace Pietra;


PPackage* PPackage::from(const char *filename){
    PPackage* package = arena_alloc<PPackage>();    
    
    PStreamCursor* Streamcursor = PStreamCursor::from(filename);
    if(!Streamcursor) return nullptr;
    
    while((Streamcursor->decl = Parser::decl())){
        package->ast.push(Streamcursor->decl);        
        
    }
    package->cursor = arena_alloc<PCursor>();
    package->cursor->setDecls(package->ast);    
    return package;

}

SVec<Decl*> PPackage::getAst(){ 
    return this->ast; 
}

Decl* PPackage::getSymFromName(const char* name){
    name = Core::cstr(name); // Intern the string 
    for(Decl* node: this->ast){
        if(node->name == name) return node;
    }
    return nullptr;
}
#endif /*PACKAGE_CPP*/