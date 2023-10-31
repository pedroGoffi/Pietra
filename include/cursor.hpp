#ifndef CURSOR_HPP
#define CURSOR_HPP
#include "ast.hpp"
#include <cstdio>


using namespace Pietra::Ast;

namespace Pietra {
    struct PStreamCursor {       
        const char* stream;
        Decl*       decl;
        
        static PStreamCursor* from(const char* filename);

        // parse the next declaration
        void next();
        // target the next file
        
        
    };

    struct PCursor {        
    private:
        SVec<Decl*>     mDecls;
        Decl*           mDecl;


    public:
        PCursor(SVec<Decl*>& Decls);
        Decl* next();
        const char* getName();        
        void setDecls(SVec<Decl*> Decls);
        Decl* find(const char* name);

    };
}
#endif /*CURSOR_HPP*/