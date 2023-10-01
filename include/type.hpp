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
    };

    struct Type{
        TypeKind    kind;
        const char* name;
        int         size;
        union {
            Type* base;
            SVec<TypeField*> aggregate_items;

            struct {
                Type*   base;
                int     size;
            } array;
            
            
            struct {
                SVec<TypeField*>    params;
                Type*               ret_type;
                bool                is_varag;
            } proc;

            struct {
                SVec<TypeField*> items;
            } aggregate;
        };
    };

    struct TypeField {
        const char* name;
        Type*       type;
    };
    TypeField* init_typefield(const char* name, Type* type);
    Type* type_init(TypeKind kind);
    Type* type_int(int size);    
    Type* type_void();
    Type* type_float(int size);
    Type* type_ptr(Type* base);
    Type* type_array(Type* base, int size);
    Type* type_struct(SVec<TypeField*> fields);
    Type* type_union(SVec<TypeField*> fields);
    Type* type_proc(SVec<TypeField*> params, Type* ret_type);
    Type* type_string();
    Type* type_aggregate(SVec<TypeField*> items, bool isStruct = true);
    Type* type_unresolved();
    Type* unqualify_type(Type* type);
    bool is_arithmetric_type(Type* type);
    bool can_add_type(Type* lhs, Type* rhs);

}
#endif /*TYPES_HPP*/