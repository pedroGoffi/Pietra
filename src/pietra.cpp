#include "../include/pietra.hpp"
// TODO: #include "preprocess.cpp"
#include "package.cpp"
#include "cursor.cpp"
#include "arena.cpp"
#include "smallVec.cpp"
#include "ast.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#include "pprint.cpp"
#include "resolve.cpp"
#include "type.cpp"
#include "llvm.cpp"
#include "cGen.cpp"
#include <cstdlib>
#include <string>

using namespace Pietra::Lexer;
using namespace Pietra::Utils;

void arenas_cleanup(){
    main_arena.free();
    ast_arena.free();
}

int Pietra::Main(int argc, char** argv){              
    assert(argc == 2);
    PPackage* package = PPackage::from(argv[1]);
    Decl* dmain = package->get_sym("main");
    

    
    SVec<Decl*> decls = resolve_ast(package->ast);
    for(Decl* decl: decls){        
        pPrint::decl(decl);
        
    }
    LLVMCompiler::compile_ast(decls);    
    arenas_cleanup();    
    exit(1);    
}