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
void pietra_run_compilation_on_input_file(bool doLog, std::string input_file, std::string output_file, Asm::COMPILER_TARGET target, bool verbose);
void pietra_setup_argparser(ArgumentParser& parser);
int pietra_check_arg_parser(ArgumentParser& parser, int argc, char** argv);
int pietra_run_preprocess_on_input_file(bool log, std::string input_file);
std::string get_or_create_output_file_name(std::string input_file, std::string output_file);
void initialize_language_context();
int process_rtpi_file(const std::string& input_file, const std::string& output_file, bool log, const ArgumentParser& parser);
int process_pi_file(const std::string& input_file, std::string& output_file, Asm::COMPILER_TARGET target, bool log);
int initialize_and_parse_arguments(int argc, char** argv, ArgumentParser& parser, bool& verbose, Asm::COMPILER_TARGET& target, std::string& input_file, std::string& output_file);
bool string_ends_with(const std::string& str, const std::string& suffix);
PPackage* load_package_from_input(const std::string& input_file);
void log_duration(const std::string& action, clock_t start, clock_t end);
void handle_file_error(const std::string& file_type, ArgumentParser& parser);
int pietra_process_input_file(const std::string& input_file, std::string& output_file, bool log, ArgumentParser& parser, Asm::COMPILER_TARGET target);
int Main(int argc, char** argv);

void pietra_setup_argparser(ArgumentParser& parser) {
    // Add arguments to the parser
    parser.add_argument("-h", "Show help message", true);  // flag argument
    parser.add_argument("-v", "Enable verbose mode", true); // flag argument
    parser.add_argument("-win32", "Set the default output for binary code to win32", true); // flag argument    
    parser.add_argument("--file", "Specify the input file"); // non-flag argument
    parser.add_argument("--output", "Specify the output file"); // optional output file argument
    parser.add_argument("--verbose", "Verbose prints in compilations steps", true);
}
int pietra_check_arg_parser(ArgumentParser& parser, int argc, char** argv) {
    // Add arguments to the parser    
    if (argc < 2) {
        parser.show_help();
        fprintf(stderr, "[ERROR]: epected input file %s <INPUT FILE> <OPTIONAL FLAGS>.\n", argv[0]);
        return 0;
    }

    // Parse command-line arguments
    if (!parser.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    // Check for help flag
    if (parser.is_flag_set("-h")) {
        parser.show_help();
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
int pietra_run_preprocess_on_input_file(bool log, std::string input_file){
    // Start measuring the time for loading the package
    clock_t start, end, all_start;
    double duration, all_duration;
    // Function to load the package from the input file
    all_start = clock();
    if(log) fprintf(stderr, "Initializing pietra compiler on %s\n", input_file.c_str());
    Pietra::PPackage* package = Pietra::PPackage::from(input_file.c_str());    

        
    // Start measuring the time for compiling the AST
    start = clock();

    // Function to compile the AST and generate the output
    std::vector<Decl*> ast;
    for(Decl* decl: package->ast){
        ast.push_back(decl);
    }
    Result result = prep_run_ast(ast);
    if(!result.is_success()){
        fprintf(stderr, "[ERROR]: %s\n", result.to_string().c_str());
        return EXIT_FAILURE;
    }

    end = clock();
    duration = double(end - start) / CLOCKS_PER_SEC;
    all_duration = double(end - all_start) / CLOCKS_PER_SEC;
    if(log) {
        fprintf(stderr, "> []: Time spent on interpretation:    %.6f seconds.\n", duration);
        fprintf(stderr, "> []: Time in the whole process:       %.6f seconds.\n", all_duration);
    }

    return EXIT_SUCCESS;
}
std::string get_or_create_output_file_name(std::string input_file, std::string output_file){
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
    return output_file;
}

void initialize_language_context(){
    initInternKeywords();
    init_prep();
    declare_built_in();    
}

int process_rtpi_file(const std::string& input_file, const std::string& output_file, bool log, const ArgumentParser& parser) {
    if (!output_file.empty()) {
        parser.show_help();
        fprintf(stderr, "[ERROR]: Output file must not be specified for preprocess files.\n");
        return EXIT_FAILURE;
    }

    if (pietra_run_preprocess_on_input_file(log, input_file) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
int process_pi_file(const std::string& input_file, std::string& output_file, Asm::COMPILER_TARGET target, bool verbose) {
    output_file = get_or_create_output_file_name(input_file, output_file);
    pietra_run_compilation_on_input_file(log, input_file, output_file, target, verbose);
    return EXIT_SUCCESS;
}
int initialize_and_parse_arguments(int argc, char** argv, ArgumentParser& parser, bool& verbose, Asm::COMPILER_TARGET& target, std::string& input_file, std::string& output_file) {
    pietra_setup_argparser(parser);
    if (pietra_check_arg_parser(parser, argc, argv) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    setResolverDebug(parser.is_flag_set("--verbose"));
    verbose = parser.is_flag_set("--verbose");
        
    target = parser.is_flag_set("-win32") 
        ? Asm::COMPILER_TARGET::CT_WINDOWS 
        : Asm::COMPILER_TARGET::CT_LINUX;

    input_file = parser.get_input_file();
    if (input_file.empty()) {
        printf("[ERROR]: Input file not specified.\n");
        return EXIT_FAILURE;
    }

    output_file = parser.get_value("--output");
    return EXIT_SUCCESS;
}

bool string_ends_with(const std::string& str, const std::string& suffix) {
    // Check if the suffix is longer than the string
    if (suffix.size() > str.size()) {
        return false;
    }
    // Compare the end of the string with the suffix
    return std::equal(
        suffix.rbegin(), 
        suffix.rend(),
        str.rbegin()        
    );
}

PPackage* load_package_from_input(const std::string& input_file) {
    if(log) fprintf(stderr, "Initializing pietra compiler on %s\n", input_file.c_str());
    Pietra::PPackage* package = Pietra::PPackage::from(input_file.c_str());
    return package;
}

void log_duration(const std::string& action, clock_t start, clock_t end) {
    double duration = double(end - start) / CLOCKS_PER_SEC;
    if(log) fprintf(stderr, "> []: Time spent in %s: %.6f seconds.\n", action.c_str(), duration);
}

void handle_file_error(const std::string& file_type, ArgumentParser& parser) {    
    parser.show_help();
    fprintf(stderr, "[ERROR]: Expected .rtpi or .pi file, but got '%s'.\n", file_type.c_str());
}

void pietra_run_compilation_on_input_file(bool doLog, std::string input_file, std::string output_file, Asm::COMPILER_TARGET target, bool verbose) {
    clock_t start, end, all_start;
    double duration, all_duration;

    all_start = clock();
    PPackage* package = load_package_from_input(input_file);    

    start = clock();
    SVec<Decl*> ast = Resolver::resolve_package(package);
    end = clock();
    log_duration("Resolving AST", start, end);

    start = clock();
    Asm::compile_ast(ast, target, output_file.c_str(), verbose);
    end = clock();
    log_duration("Compilation", start, end);

    all_duration = double(end - all_start) / CLOCKS_PER_SEC;
    if(log) fprintf(stderr, "> []: Time in the whole process: %.6f seconds.\n", all_duration);
}

int pietra_process_input_file(const std::string& input_file, 
                       std::string& output_file, 
                       bool verbose, 
                       ArgumentParser& parser, 
                       Asm::COMPILER_TARGET target) {
                        
    if (string_ends_with(input_file, ".rtpi")) {
        return process_rtpi_file(input_file, output_file, verbose, parser);
    } 
    else if (string_ends_with(input_file, ".pi")) {
        return process_pi_file(input_file, output_file, target, verbose);
    } 
    else {
        handle_file_error(input_file, parser);
        return EXIT_FAILURE;
    }
}

int Main(int argc, char** argv) {
    ArgumentParser parser;
    bool verbose = false;
    Asm::COMPILER_TARGET target = Asm::get_current_platform_compiler_target();
    std::string input_file, output_file;

    if (initialize_and_parse_arguments(argc, argv, parser, verbose, target, input_file, output_file) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    
    initialize_language_context();    
    if(pietra_process_input_file(input_file, output_file, verbose, parser, target) == EXIT_FAILURE){
        fprintf(stderr, "[ERROR]: Pietra lang failed to process the file '%s'.\n", input_file.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
}