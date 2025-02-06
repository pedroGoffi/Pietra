#ifndef LLVM_DECL_COMPILER
#define LLVM_DECL_COMPILER
#include "llvm-includes.cpp"
#include "llvm-statements.cpp"
#include "llvm-types.cpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
llvm::Function *compileDeclProcPrototype(Decl *decl, Ctx &ctx) {
  assert(decl && "Declaration cannot be null");
  printf("COMPILING DECL PROC PROTOTYPE %s\n", decl->name);

  if (decl->kind != DECL_PROC) {
    return nullptr; // Não é uma declaração de procedimento
  }

  // Mapear o tipo de retorno para LLVM
  llvm::Type *retType = mapTypeToLLVM(decl->proc.ret->resolvedTy);

  // Coletar tipos de parâmetros
  std::vector<llvm::Type *> paramTypes;
  for (auto &param : decl->proc.params) {
    paramTypes.push_back(mapTypeToLLVM(param->type->resolvedTy));
  }

  // Definir se a função é vararg
  bool isVararg = decl->proc.is_vararg;
  llvm::FunctionType *funcType =
      llvm::FunctionType::get(retType, paramTypes, isVararg);

  // Criar a função no módulo
  llvm::Function *func = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, decl->name, *mod);

  // Nomear os parâmetros
  auto argIt = func->arg_begin();
  for (auto &param : decl->proc.params) {
    Sym *var = sym_var(param->name, param->type->resolvedTy, param->init);
    ctx.getLocalScope()->add_sym(var);
    argIt->setName(param->name);
    ++argIt;
  }

  return func;
}

void compileDeclProcBody(Decl *decl, llvm::Function *func, Ctx &ctx) {
  assert(decl && "Declaration cannot be null");
  assert(func && "Function must be valid");

  if (!decl->proc.is_complete or decl->proc.is_internal)
    return;

  // Criar bloco de entrada e definir ponto de inserção do Builder
  llvm::BasicBlock *entryBB =
      llvm::BasicBlock::Create(*LLVMcontext.get(), "entry", func);
  builder->SetInsertPoint(entryBB);

  // Compilar as instruções do corpo do procedimento
  ctx.getLocalScope()->enter_scope();
  for (auto &stmt : decl->proc.block) {
    compileStmt(stmt, ctx);
  }
  ctx.clearCurrentProcedure();
}

llvm::Function *compileDeclProc(Decl *decl, Ctx &ctx) {
  // Compila o protótipo da função
  llvm::Function *func = compileDeclProcPrototype(decl, ctx);
  if (!func)
    return nullptr;

  // Compila o corpo da função
  compileDeclProcBody(decl, func, ctx);

  return func;
}

llvm::Type *compileDeclAggregate(Decl *decl, Ctx &ctx) {
  assert(decl && "Decl cannot be null");
  assert(decl->kind == DECL_AGGREGATE && "Expected an aggregate declaration");
  assert(ctx.getGlobalScope()->find_sym(decl->name));

  llvm::Type *aggregateType = nullptr;

  // Check if the type already exists in the module
  if (auto existingType =
          llvm::StructType::getTypeByName(*LLVMcontext.get(), decl->name)) {
    return existingType;
  }

  if (decl->aggregate.aggkind == AGG_STRUCT) {
    // For a struct: Create a struct type in LLVM
    std::vector<llvm::Type *> fieldTypes;

    // Iterate over the items (fields) of the aggregate
    for (auto &item : decl->aggregate.items) {
      assert(item->kind == AGGREGATEITEM_FIELD &&
             "Expected a field in the aggregate");

      // Map the field type to the LLVM type
      llvm::Type *fieldType = mapTypeToLLVM(item->field.type->resolvedTy);

      // Add the field type to the list of types
      fieldTypes.push_back(fieldType);
    }

    // Create the struct type in LLVM with the field types
    aggregateType =
        llvm::StructType::create(*LLVMcontext.get(), fieldTypes, decl->name);

  } else if (decl->aggregate.aggkind == AGG_UNION) {
    // For a union: Create a union type in LLVM
    std::vector<llvm::Type *> fieldTypes;
    llvm::Type *largestType = nullptr;

    // Iterate over the items (fields) of the aggregate
    for (auto &item : decl->aggregate.items) {
      assert(item->kind == AGGREGATEITEM_FIELD &&
             "Expected a field in the aggregate");

      // Map the field type to the LLVM type
      llvm::Type *fieldType = mapTypeToLLVM(item->field.type->resolvedTy);

      // Add the field type to the list
      fieldTypes.push_back(fieldType);

      // Find the largest type for the union
      if (!largestType || fieldType->getScalarSizeInBits() >
                              largestType->getScalarSizeInBits()) {
        largestType = fieldType;
      }
    }

    // Create the union type in LLVM
    aggregateType =
        llvm::StructType::create(*LLVMcontext.get(), fieldTypes, decl->name);

    // Ensure the union has the largest possible size
    aggregateType = llvm::PointerType::getUnqual(aggregateType);
  } else {
    // If the aggregate kind is not supported
    assert(false && "Unsupported aggregate kind");
  }

  return aggregateType;
}

llvm::Value *compileDecl(Decl *decl, Ctx &ctx) {
  assert(decl && "Declaration cannot be null");

  switch (decl->kind) {
  case DECL_PROC:
    return compileDeclProc(decl, ctx);
  case DECL_AGGREGATE:
    compileDeclAggregate(decl, ctx);
    return nullptr;
  default:
    assert(false && "LLVM compilation for this declaration is not implemented");
  }
}

#endif /* LLVM_DECL_COMPILER */
