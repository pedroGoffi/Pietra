#ifndef CURSOR_CPP
#define CURSOR_CPP
#include "../include/cursor.hpp"
#include "../include/arena.hpp"
#include "../include/file.hpp"

using namespace Pietra;


Lexer* PStreamCursor::getLexer(){
    return this->lexer;
}

PStreamCursor* PStreamCursor::from(const char *filename){    
    PStreamCursor* cursor = Core::arena_alloc<PStreamCursor>();
    cursor->lexer = Lexer::open_at_file(filename);
    cursor->stream = cursor->lexer->getPtr();    
    return cursor;

}

void PStreamCursor::next(){    
    this->decl = Parser::decl(this->lexer);    
}

PCursor::PCursor(SVec<Decl*>& Decls)
   : mDecls(Decls){}
   
Decl* PCursor::next(){
   this->mDecl =  this->mDecls.next();
   return this->mDecl;
}
const char* PCursor::getName(){
    assert(this->mDecl);    
    return this->mDecl->name;
}

void PCursor::setDecls(SVec<Decl*> Decls){
   this->mDecls = Decls;
   this->mDecl  = nullptr;
   
   if(Decls.len() > 0){
        this->mDecl = this->mDecls.at(0);
   }   
}
Decl* PCursor::find(const char* name){    
    name = Core::cstr(name); // Makign intern string
    for(auto& node: this->mDecls){
        if(name == node->name){
            return node;
        }
    }
    return nullptr;
}
#endif /*CURSOR_CPP*/