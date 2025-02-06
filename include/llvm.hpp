#ifndef  LLVM_COMPILER_HPP
#define  LLVM_COMPILER_HPP

#include "arena.hpp"
#include "ast.hpp"
#include "resolve.hpp"
#include "smallVec.hpp"
#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>
#include "bridge.hpp"
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#endif /*LLVM_COMPILER_HPP*/