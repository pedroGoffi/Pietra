#ifndef CURSOR_HPP
#define CURSOR_HPP
#include "ast.hpp"
#include "resolve.hpp"
#include <cstdio>


using namespace Pietra::Resolver;
namespace Pietra {
    struct PCursor {       
        const char* stream;
        Decl* decl;
        
        static PCursor* from(const char* filename);

        // parse the next declaration
        void next();
        // target the next file
        
        
    };
}
#endif /*CURSOR_HPP*/