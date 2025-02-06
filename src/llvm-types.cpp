#ifndef LLVM_TYPES
#define LLVM_TYPES
#include "llvm-globals.cpp"
#include "llvm-includes.cpp"
llvm::Type *mapTypeToLLVM(Type *type) {
  switch (type->kind) {
  case TYPE_I8:
    return llvm::Type::getInt8Ty(*LLVMcontext.get());
  case TYPE_I16:
    return llvm::Type::getInt16Ty(*LLVMcontext);
  case TYPE_I32:
    return llvm::Type::getInt32Ty(*LLVMcontext);
  case TYPE_I64:
    return llvm::Type::getInt64Ty(*LLVMcontext);
  case TYPE_F32:
    return llvm::Type::getFloatTy(*LLVMcontext);
  case TYPE_F64:
    return llvm::Type::getDoubleTy(*LLVMcontext);
  case TYPE_PTR:
    // Mapeia para ponteiro do tipo base
    return llvm::PointerType::getUnqual(mapTypeToLLVM(type->base));
  case TYPE_ARRAY:
    // Mapeia para array
    return llvm::ArrayType::get(mapTypeToLLVM(type->array.base),
                                type->array.size);
  case TYPE_STRUCT: {
    // Mapeia para struct
    std::vector<llvm::Type *> llvmFields;
    for (auto field : type->aggregate.items) {
      llvmFields.push_back(mapTypeToLLVM(field->type));
    }
    return llvm::StructType::create(*LLVMcontext, llvmFields, type->name);
  }
  case TYPE_UNION: {
    // Mapeia para union, que no LLVM é basicamente um struct com alinhamento
    // especial
    std::vector<llvm::Type *> llvmFields;
    for (auto field : type->aggregate.items) {
      llvmFields.push_back(mapTypeToLLVM(field->type));
    }
    return llvm::StructType::create(*LLVMcontext, llvmFields, type->name,
                                    /*packed=*/true);
  }
  case TYPE_PROC:
    // Funções podem ser representadas como ponteiros para funções
    {
      std::vector<llvm::Type *> paramTypes;
      for (auto param : type->proc.params) {
        paramTypes.push_back(mapTypeToLLVM(param->type));
      }

      llvm::Type *retType = mapTypeToLLVM(type->proc.ret_type);
      return llvm::FunctionType::get(retType, paramTypes, type->proc.is_vararg);
    }
  case TYPE_ENUM:
    // Enum pode ser mapeado para um tipo inteiro (por exemplo, i32)
    return llvm::Type::getInt32Ty(*LLVMcontext);
  case TYPE_VOID:
    return llvm::Type::getVoidTy(*LLVMcontext);
  case TYPE_ANY:
    // Tipo genérico pode ser tratado como um ponteiro void
    return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(*LLVMcontext));

  default:
    return nullptr; // Tipo não mapeado
  }
}

#endif /*LLVM_TYPES*/
