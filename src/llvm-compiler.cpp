#ifndef LLVM_COMPILER
#define LLVM_COMPILER

#include "llvm-decl.cpp"
#include "llvm-expressions.cpp"
#include "llvm-includes.cpp"
#include "llvm-statements.cpp"
#include "llvm-types.cpp"

void compiler_checkup() {
  assert(LLVMcontext != nullptr);
  assert(mod != nullptr);
  assert(builder != nullptr);
}
int compile_ast_to_llvm(SVec<Decl *> ast, Asm::COMPILER_TARGET target,
                        const char *output_file, bool verbose) {
  ctx.getGlobalScope()->enter_scope();
  compiler_checkup();
  for (Decl *decl : ast) {
    llvm::Value *value = compileDecl(decl, ctx);
    if (value) {
      value->print(llvm::errs(), true);
      llvm::errs() << "\n";
    }
  }

  // Finalize module and output to file
  std::error_code error;
  llvm::raw_fd_ostream output_stream(output_file, error,
                                     llvm::sys::fs::OF_None);

  if (error) {
    std::cerr << "Error opening output file: " << error.message() << std::endl;
    return EXIT_FAILURE;
  }

  mod->print(output_stream, nullptr);
  return EXIT_SUCCESS;
}

#endif /* LLVM_COMPILER */
