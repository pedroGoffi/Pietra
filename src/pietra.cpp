#include "../include/pietra.hpp"
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
//#include "llvm.cpp"



void init_flags(){                     
    init_or_get_flag(FLAG_NAME_IMPLICIT_CAST, true);
}

int Pietra::Main(int argc, char** argv){              
    init_flags();

    
    assert(argc == 2);
    declare_built_in();          
    PPackage* package = PPackage::from(argv[1]);        
    if(!package){
        printf("[ERROR]: Could not find the package %s\n", argv[1]);
        exit(1);
    }    
    SVec<Decl*> ast = resolve_package(package);
       
    Asm::compile_ast(ast);
    //LLVMCompiler::compile_ast(ast);        
    
    if (DEBUG_MODE) printf("Pietra compiled successfuly.\n");
    if (DEBUG_MODE) printf("Pietra Arena usage: %zu bytes\n", main_arena.size);

    arena_free();
    return EXIT_SUCCESS;
}