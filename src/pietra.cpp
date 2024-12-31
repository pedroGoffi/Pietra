#include "../include/pietra.hpp"
#include "preprocess.cpp"
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
#include "context.cpp"
#include "Asmx86_64.cpp"
#include "argparser.cpp"

namespace Pietra {
int Main(int argc, char** argv) {
    ArgumentParser parser;

    // Add arguments to the parser
    parser.add_argument("-h", "Show help message", true);  // flag argument
    parser.add_argument("-v", "Enable verbose mode", true); // flag argument
    parser.add_argument("-win32", "Set the default output for binary code to win32", true); // flag argument    
    parser.add_argument("--file", "Specify the input file"); // non-flag argument
    parser.add_argument("--output", "Specify the output file"); // optional output file argument
    parser.add_argument("--log", "Logs the performance of the compiler", true);


    if (argc < 2) {
        parser.show_help();
        fprintf(stderr, "[ERROR]: epected input file %s <INPUT FILE> <OPTIONAL FLAGS>.\n", argv[0]);
        return 0;
    }

    // Parse command-line arguments
    if (!parser.parse(argc, argv)) {
        return 1;
    }

    // Check for help flag
    if (parser.is_flag_set("-h")) {
        parser.show_help();
        
        return 0;
    }

    // Verbose flag
    setResolverDebug(parser.is_flag_set("-v"));
    bool log = parser.is_flag_set("--log");

    // Check for win32 flag and set the target accordingly
    Asm::COMPILER_TARGET target = Asm::COMPILER_TARGET::CT_LINUX;
    if (parser.is_flag_set("-win32")) {
        target = Asm::COMPILER_TARGET::CT_WINDOWS;
    }

    // Get the input file from the argument or set to default if not provided
    std::string input_file = parser.get_input_file();
    if (input_file.empty()) {
        printf("[ERROR]: Input file not specified.\n");
        return 1;
    }

    // Generate the output file name from --output flag or default to <input_file>.bin
    std::string output_file = parser.get_value("--output");
    if (output_file.empty()) {
        // If no output file provided, create the default output file name in the current directory
        // Extract the filename from the input path (remove subfolders)
        std::filesystem::path input_path(input_file);
        std::string filename = input_path.filename().string();  // Get the filename without subfolders
        size_t ext_pos = filename.find_last_of(".");
        if (ext_pos != std::string::npos) {
            filename = filename.substr(0, ext_pos);  // Remove the file extension
        }
        output_file = filename + ".bin";  // Default to .bin extension
    }



    initInternKeywords();
    init_prep();
    declare_built_in();
    // Start measuring the time for loading the package
    clock_t start, end, all_start;
    double duration, all_duration;

    // Function to load the package from the input file
    all_start = clock();
    if(log) printf("Initializing pietra compiler on %s\n", input_file.c_str());
    Pietra::PPackage* package = Pietra::PPackage::from(input_file.c_str());    

    // Start measuring the time for resolving the package
    start = clock();
    // Function to resolve the package and generate the AST
    SVec<Decl*> ast = Resolver::resolve_package(package);

    end = clock();
    duration = double(end - start) / CLOCKS_PER_SEC;
    if(log) fprintf(stderr, "> []: Time spent in Resolving ast: %.6f seconds.\n", duration);

    // Start measuring the time for compiling the AST
    start = clock();

    // Function to compile the AST and generate the output
    Asm::compile_ast(ast, target, output_file.c_str());

    end = clock();
    duration = double(end - start) / CLOCKS_PER_SEC;
    all_duration = double(end - all_start) / CLOCKS_PER_SEC;
    if(log) {
        fprintf(stderr, "> []: Time spent Compilation:      %.6f seconds.\n", duration);
        fprintf(stderr, "> []: Time in the whole process:   %.6f seconds.\n", all_duration);
    }
    return EXIT_SUCCESS;
}


}