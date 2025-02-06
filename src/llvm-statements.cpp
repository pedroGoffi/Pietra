#ifndef LLVM_COMPILER_STMT
#define LLVM_COMPILER_STMT

#include "llvm-includes.cpp"
#include "llvm-expressions.cpp"

llvm::Value* compileStmtReturn(Stmt* stmt, Ctx& ctx) {
    assert(stmt && "Return statement cannot be null");

    // Obter a função atual
    llvm::Function* currentFunc = builder->GetInsertBlock()->getParent();
    llvm::Type* returnType = currentFunc->getReturnType();

    // Se houver uma expressão de retorno, compilá-la
    if (stmt->expr) {
        llvm::Value* returnValue = compileExpr(stmt->expr, ctx);
        return returnType->isVoidTy() ? (builder->CreateRetVoid(), nullptr)
                                      : (builder->CreateRet(returnValue), nullptr);
    }

    // Se não houver expressão, retornar void
    builder->CreateRetVoid();
    return nullptr;
}

llvm::Value* compileStmt(Stmt* stmt, Ctx& ctx) {
    assert(stmt && "Statement cannot be null");

    switch (stmt->kind) {
        case STMT_EXPR:   return compileExpr(stmt->expr, ctx);
        case STMT_RETURN: return compileStmtReturn(stmt, ctx);
        default: assert(false && "Unimplemented statement");
    }
}

#endif /* LLVM_COMPILER_STMT */
