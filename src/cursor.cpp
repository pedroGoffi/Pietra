#ifndef CURSOR_CPP
#define CURSOR_CPP
#include "../include/cursor.hpp"
#include "../include/arena.hpp"
#include "../include/file.hpp"
#include "parser.cpp"
using namespace Pietra;


PStreamCursor* PStreamCursor::from(const char *filename){    
    std::FILE* fd = fopen(filename, "r");
    if(!fd){
        printf("[ERROR]: Couldn't open the file: '%s'\n", filename);
        return nullptr;
    }
    
    PStreamCursor* cursor = Core::arena_alloc<PStreamCursor>();
    cursor->stream = fileReader::read_file(filename);
    assert(cursor->stream);
        
    init_stream(filename, cursor->stream);        
    return cursor;

}

void PStreamCursor::next(){    
    this->decl = Parser::decl();    
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