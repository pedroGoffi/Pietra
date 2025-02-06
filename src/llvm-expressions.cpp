#ifndef LLVM_COMPILER_EXPR
#define LLVM_COMPILER_EXPR

#include "./llvm-types.cpp"
#include "llvm-includes.cpp"
#include <cassert>
std::vector<Expr *> compilationStack;

int stack_size() { return compilationStack.size(); }
int stack_expr(Expr *expr) {
  compilationStack.push_back(expr);
  return stack_size();
}

int pop_expr() {
  compilationStack.pop_back();
  return stack_size() + 1;
}

Expr *current_expr() {
  if (compilationStack.size() >= 2) {
    return compilationStack.at(compilationStack.size() - 2);
  }

  return nullptr;
}
llvm::Value *compileExpr(Expr *expr, Ctx &ctx);

llvm::Value *compileExprInt(Expr *expr, Ctx &ctx) {
  (void)ctx;
  return llvm::ConstantInt::get(*LLVMcontext, llvm::APInt(32, expr->int_lit));
}

llvm::Value *compileExprString(Expr *expr, Ctx &ctx) {
  llvm::Constant *strConst =
      llvm::ConstantDataArray::getString(*LLVMcontext, expr->string_lit, true);

  auto *globalString = new llvm::GlobalVariable(
      *mod, strConst->getType(), true, llvm::GlobalValue::PrivateLinkage,
      strConst, expr->string_lit);

  llvm::PointerType *ptrType =
      llvm::PointerType::get(llvm::Type::getInt8Ty(*LLVMcontext), 0);
  return builder->CreateBitCast(globalString, ptrType);
}
llvm::Value *compileExprInitVar(Expr *expr, Ctx &ctx) {
  assert(expr && expr->kind == EXPR_INIT_VAR);
  const char *varName = expr->init_var.name;
  Type *type = expr->init_var.type ? expr->init_var.type->resolvedTy : nullptr;
  Expr *init = expr->init_var.rhs;
  llvm::Type *llvmType = mapTypeToLLVM(type);
  llvmType->print(llvm::errs());
  assert(varName && type && llvmType);

  // Create symbol and add to scope

  // Create alloca for the variable
  llvm::Value *alloca = builder->CreateAlloca(llvmType, nullptr, varName);
  Sym *sym = sym_var(varName, type, init);
  sym->llvmValue = alloca;
  ctx.getLocalScope()->add_sym(sym);

  // Compile the initializer expression
  if (init) {
    llvm::Value *initValue = compileExpr(init, ctx);
    // Store the initial value into the alloca
    builder->CreateStore(initValue, alloca);
  }

  return alloca; // Return the alloca or another appropriate value
}

llvm::Value *compileBinaryOp(llvm::Value *lhs, llvm::Value *rhs, Ctx &ctx,
                             llvm::Instruction::BinaryOps intOp,
                             llvm::Instruction::BinaryOps floatOp) {
  llvm::Type *type = lhs->getType();
  if (type->isIntegerTy()) {
    return builder->CreateBinOp(intOp, lhs, rhs, "tmp");
  } else if (type->isFloatingPointTy()) {
    return builder->CreateBinOp(floatOp, lhs, rhs, "ftmp");
  }
  assert(false && "Unsupported operand type");
  return nullptr;
}
llvm::Value *compileAssignVarExpr(Expr *expr, Ctx &ctx) {
  assert(expr->binary.binary_kind == TK_eq);
  llvm::Value *lhs = compileExpr(expr->binary.left, ctx);
  llvm::Value *rhs = compileExpr(expr->binary.right, ctx);

  return builder->CreateStore(lhs, rhs);
}
llvm::Value *compileExprBinary(Expr *expr, Ctx &ctx) {
  assert(expr->kind == EXPR_BINARY && "Expected a binary expression");
  if (expr->binary.binary_kind == TK_eq)
    return compileAssignVarExpr(expr, ctx);

  llvm::Value *lhs = compileExpr(expr->binary.left, ctx);
  llvm::Value *rhs = compileExpr(expr->binary.right, ctx);

  switch (expr->binary.binary_kind) {
  case TK_plus:
    return compileBinaryOp(lhs, rhs, ctx, llvm::Instruction::Add,
                           llvm::Instruction::FAdd);
  case TK_less:
    return compileBinaryOp(lhs, rhs, ctx, llvm::Instruction::Sub,
                           llvm::Instruction::FSub);
  case TK_mul:
    return compileBinaryOp(lhs, rhs, ctx, llvm::Instruction::Mul,
                           llvm::Instruction::FMul);
  case TK_div:
    return compileBinaryOp(lhs, rhs, ctx, llvm::Instruction::SDiv,
                           llvm::Instruction::FDiv);

  default:
    assert(false && "Unknown binary operator");
    return nullptr;
  }
}
bool shouldLoadExpr(Expr *expr) {
  switch (expr->kind) {
  case EXPR_ARRAY_INDEX:
  case EXPR_CALL:
    return true; // Load value

  case EXPR_FIELD:
    return false;

  case EXPR_BINARY:
    return expr->binary.binary_kind == TK_eq;

  default:
    return true; // Safe fallback: load value
  }
}

llvm::Value *compileExprName(Expr *expr, Ctx &ctx) {
  // TODO: fix load conditions
  const char *varName = expr->name;
  Sym *sym = nullptr;
  if (auto *scope = ctx.getLocalScope()) {
    sym = scope->find_sym(varName);
  }

  if (!sym) {
    printf("[ERROR]: symbol %s not found\n", varName);
    exit(1);
  }
  assert(sym && "Variable not found in symbol table");
  assert(sym->llvmValue);

  Expr *curr_expr = current_expr();
  if (curr_expr and shouldLoadExpr(curr_expr)) {
    llvm::Type *loadType = mapTypeToLLVM(sym->type);
    return builder->CreateLoad(loadType, sym->llvmValue, varName);
  }
  return sym->llvmValue;
}

llvm::Value *compileExprField(Expr *expr, Ctx &ctx) {
  assert(expr->kind == EXPR_FIELD);
  assert(expr->field_acc.parent->kind == EXPR_NAME);
  llvm::Value *parent = compileExpr(expr->field_acc.parent, ctx);
  Sym *var = ctx.getLocalScope()->find_sym(expr->field_acc.parent->name);
  assert(expr->field_acc.children->kind == EXPR_NAME);
  const char *fieldName = expr->field_acc.children->name;

  llvm::Type *structType = mapTypeToLLVM(var->type);
  int fieldIndex = 0;
  assert(var->type->kind == TYPE_STRUCT);

  bool fieldFound = false;
  for (TypeField *tf : var->type->aggregate.items) {
    if (tf->name == fieldName) {
      fieldFound = true;
      break;
    }
    fieldIndex++;
  }
  assert(fieldFound);

  return builder->CreateStructGEP(structType, parent, fieldIndex);
}

llvm::Value *compileExpr(Expr *expr, Ctx &ctx) {
  assert(expr);
  int prev = stack_expr(expr);
  llvm::Value *value;
  switch (expr->kind) {
  case EXPR_INT:
    value = compileExprInt(expr, ctx);
    break;
  case EXPR_STRING:
    value = compileExprString(expr, ctx);
    break;
  case EXPR_INIT_VAR:
    value = compileExprInitVar(expr, ctx);
    break;
  case EXPR_NAME:
    value = compileExprName(expr, ctx);
    break;
  case EXPR_BINARY:
    value = compileExprBinary(expr, ctx);
    break;
  case EXPR_FIELD:
    value = compileExprField(expr, ctx);
    break;
  default:
    assert(false && "Unimplemented expression");
  }
  int curr = pop_expr();

  if (prev != curr) {
    printf("[ERROR]: stack context size bad logic. [%i, %i]\n", prev, curr);
    exit(1);
  }
  return value;
}

#endif /* LLVM_COMPILER_EXPR */
