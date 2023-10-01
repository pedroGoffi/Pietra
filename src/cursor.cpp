#ifndef CURSOR_CPP
#define CURSOR_CPP
#include "../include/cursor.hpp"
#include "../include/arena.hpp"
#include "../include/file.hpp"
#include "parser.cpp"
using namespace Pietra;


PCursor* PCursor::from(const char *filename){    
    std::FILE* fd = fopen(filename, "r");
    if(!fd){
        return nullptr;
    }
    
    PCursor* cursor = Core::arena_alloc<PCursor>();
    cursor->stream = fileReader::read_file(filename);
    assert(cursor->stream);
        
    init_stream(cursor->stream);        
    return cursor;

}

void PCursor::next(){    
    this->decl = Parser::decl();    
}

#endif /*CURSOR_CPP*/