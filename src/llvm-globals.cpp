#ifndef LLVM_GLOBALS
#define LLVM_GLOBALS
#include "./llvm-includes.cpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>

using Ctx = CBridge::Context::AssemblyContext;

std::unique_ptr<llvm::LLVMContext> LLVMcontext =
    std::make_unique<llvm::LLVMContext>();

std::unique_ptr<llvm::Module> mod =
    std::make_unique<llvm::Module>("PLang", *LLVMcontext.get());

std::unique_ptr<llvm::IRBuilder<>> builder =
    std::make_unique<llvm::IRBuilder<>>(*LLVMcontext.get());
#endif /*LLVM_GLOBALS*/
