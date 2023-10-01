#ifndef INTERN_DATA
#define INTERN_DATA
#include "../include/interns.hpp"
#include "arena.cpp"
#include "cGen.cpp"
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <cstdarg>

using namespace Pietra;
using namespace Pietra::Core;

SVec<Intern_string*> intern_strs;
// STRING TRAITS


char *strf(const char *fmt, ...) {
    va_list args;
    
    va_start(args, fmt);
    size_t n = 1 + vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* str = arena_alloc<char>(n);     
    va_start(args, fmt);
    vsnprintf(str, n, fmt, args);
    va_end(args);    
    return str;
}


const char* Core::cstr_range(const char *begin, const char *end){
    size_t len = end - begin;
    char* str = strf("%.*s", len, begin);
    for(Intern_string* intern: intern_strs){                
        if(intern->len == len and !strncmp(intern->data, begin, len)){            
            return intern->data;
        }
    }
            
    Intern_string* intern   = new Intern_string;
    intern->data            = str;
    intern->len             = len;    
    intern_strs.push(intern);
    return intern->data;                    
}

const char* Core::cstr(const char* str){
    return Core::cstr_range(str, str + strlen(str));
}
#endif /*INTERN_DATA*/