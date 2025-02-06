#ifndef LLVM_INCLUDES
#define LLVM_INCLUDES
#include "../include/Asmx86_64.hpp"
#include "../include/llvm.hpp"
#include "arena.cpp"
#include "bridge.cpp"
#include "dev_debug.cpp"
#include "pprint.cpp"
#include "resolve.cpp"
#include "llvm/IR/LLVMContext.h" // For llvm::LLVMContext
#include "llvm/IR/Module.h"      // For llvm::Module
#include "llvm/IR/Type.h"        // For llvm::Type
#include <csetjmp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <ostream>
#endif /*LLVM_INCLUDES*/
