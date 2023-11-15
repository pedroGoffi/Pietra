#ifndef PACKAGES_HPP
#define PACKAGES_HPP

#include "smallVec.hpp"
#include "arena.hpp"
#include "ast.hpp"
#include "cursor.hpp"
#include "interns.hpp"
#include "parser.hpp"
using namespace Pietra::Core;


namespace Pietra {
    struct PPackage {        
        const char* name;
        PCursor*    cursor;        
        Decl*       at;
        SVec<Decl*> ast;
        

        static PPackage* from(const char* filename);
        Decl*        getSymFromName(const char* name);
        SVec<Decl*>  getAst();        
    };
}
#endif /*PACKAGES_HPP*/