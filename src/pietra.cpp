#include "../include/pietra.hpp"
#include <fcntl.h>
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
#include "preprocess.cpp"
#include "Asmx86_64.cpp"
//#include "GFSL_compiler.cpp"
//#include "llvm.cpp"
#


using namespace Pietra::Lexer;
using namespace Pietra::Utils;



int Pietra::Main(int argc, char** argv){              
    assert(argc == 2);
    
    PPackage* package = PPackage::from(argv[1]);        
    SVec<Decl*> ast = resolve_package(package);
    
    Asm::compile_ast(ast);
    
    if (DEBUG_MODE) printf("Pietra compiled successfuly.\n");
    if (DEBUG_MODE) printf("Pietra Arena usage: %zu bytes\n", main_arena.size);
    arena_free();            
    return EXIT_SUCCESS;
}