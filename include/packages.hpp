#ifndef PACKAGES_HPP
#define PACKAGES_HPP

#include "smallVec.hpp"
#include "arena.hpp"
#include "ast.hpp"
#include "resolve.hpp"
#include "cursor.hpp"
#include "interns.hpp"
#include "parser.hpp"
using namespace Pietra::Core;

namespace Pietra {
    struct PPackage {
        PCursor*    cursor;        
        SVec<Decl*> ast;


        static PPackage* from(const char* filename);
        Decl*        get_sym(const char* name);
        SVec<Decl*>  get_all();
    };
}
#endif /*PACKAGES_HPP*/