#ifndef TYPES_HPP
#define TYPES_HPP
#include "smallVec.hpp"
#include "ast.hpp"


namespace Pietra::Ast {
    using namespace Pietra::Core;

    struct TypeField;
    struct Type;

    enum TypeKind {
        TYPE_NONE,
        TYPE_UNRESOLVED,
        TYPE_VOID,        
        TYPE_ANY,
        TYPE_FIRST_ARITHMETRIC_TYPE,
            TYPE_I8,
            TYPE_I16,
            TYPE_I32,
            TYPE_I64,
            TYPE_F32,
            TYPE_F64,
        TYPE_LAST_ARITHMETRIC_TYPE,
        TYPE_PTR,
        TYPE_ARRAY,
        TYPE_STRUCT,
        TYPE_UNION,
        TYPE_PROC,
        TYPE_CONST,
        TYPE_SELF,
        TYPE_ENUM
    };

    struct Type{
        TypeKind    kind;
        const char* name;
        int         size;        
        bool        isSelf;
        union {
            Type* base;
            
            struct {
                Type*   base;
                int     size;
            } array;
            
            
            struct {
                SVec<TypeField*>    params;
                Type*               ret_type;
                bool                is_vararg;
            } proc;

            struct {
                bool                isUnion;                
                size_t              size;
                SVec<TypeField*>    items;
            } aggregate;            
        };

        bool typeCheck(Type* other);
        const char* repr();
        bool isCallable();
        bool isLValue();
        bool canCast(Type* type);
        int calcSize();
    };

    struct TypeField {
        const char* name;
        Type*       type;
    };
    TypeField* init_typefield(const char* name, Type* type);
    Type* type_init(TypeKind kind);
    Type* type_int(int size);    
    Type* type_void();
    Type* type_any();
    Type* type_float(int size);
    Type* type_ptr(Type* base);
    Type* type_array(Type* base, int size);
    Type* type_struct(SVec<TypeField*> fields);
    Type* type_self();
    Type* type_union(SVec<TypeField*> fields);
    Type* type_proc(const char* name, SVec<TypeField*> params, Type* ret_type, bool is_vararg);
    Type* type_string();
    Type* type_aggregate(SVec<TypeField*> items, bool isStruct = true);
    Type* type_unresolved();
    Type* unqualify_type(Type* type);
    bool isNumericType(Type* type);
    bool can_add_type(Type* lhs, Type* rhs);
    bool TypeCheck(Type* lhs, Type* rhs);    
    Type* biggestNumericType(Type* lhs, Type* rhs);
}
#endif /*TYPES_HPP*/