#ifndef  LLVM_COMPILER_CPP
#define  LLVM_COMPILER_CPP
#include "../include/llvm.hpp"
#include "arena.cpp"
#include "pprint.cpp"
#include "resolve.cpp"
#include <csetjmp>
#include <cstdint>
#include <fstream>
#include <ios>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <ostream>
#include <iostream>
#include "dev_debug.cpp"

using namespace Resolver;
std::unique_ptr<llvm::LLVMContext>   ctx       = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module>        mod       = std::make_unique<llvm::Module>("PLang", *ctx);    
std::unique_ptr<llvm::IRBuilder<>>   builder   = std::make_unique<llvm::IRBuilder<>>(*ctx);

