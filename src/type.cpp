#ifndef TYPE_CPP
#define TYPE_CPP
#include "../include/type.hpp"
#include <cstdint>
#include "arena.cpp"

using namespace Pietra;
using namespace Pietra::Ast;
using namespace Pietra::Core;

// TODO: cached types for faster compilation


Type int8_ty        = {.kind = TYPE_I8,     .size = sizeof(int8_t)};
Type int16_ty       = {.kind = TYPE_I16,    .size = sizeof(int16_t)};
Type int32_ty       = {.kind = TYPE_I32,    .size = sizeof(int32_t)};
Type int64_ty       = {.kind = TYPE_I64,    .size = sizeof(int64_t)};
Type f32_ty         = {.kind = TYPE_F32,    .size = sizeof(float)};
Type f64_ty         = {.kind = TYPE_F64,    .size = sizeof(double)};
Type str_ty         = {.kind = TYPE_PTR,    .size = sizeof(void*), .base = &int8_ty};
Type void_ty        = {.kind = TYPE_VOID,   .size = 0};
Type unresolved_ty  = {.kind = TYPE_UNRESOLVED};





TypeField* Ast::init_typefield(const char* name, Type* type){
    TypeField* tf = Core::arena_alloc<TypeField>();
    tf->name = name;
    tf->type = type;
    return tf;
}
Type* Ast::type_init(TypeKind kind){
    Type* t = Core::arena_alloc<Type>();    
    t->kind = kind;
    return t;
}
Type* Ast::type_void(){
    return &void_ty;
}
Type* Ast::type_int(int size = 64){
    assert(size == 8 or size == 16 or size == 32 or size == 64);
    switch(size){
        case 8:     return &int8_ty;
        case 16:    return &int16_ty;
        case 32:    return &int32_ty;
        case 64:    return &int64_ty;
    }
    assert(0);        
}
Type* Ast::type_float(int size = 64){
    assert(size == 32 or size == 64);
    
    if(size == 64) {
        return &f64_ty;
    } else {
        return &f32_ty;
    }
}
Type* Ast::type_ptr(Type* base){
    Type* t = type_init(TYPE_PTR);
    assert(sizeof(void*) == 8);
    t->size = sizeof(void*);
    t->base = base;
    return t;
}
Type* Ast::type_string(){
    return &str_ty;
}
Type* Ast::type_array(Type* base, int size){
    Type* t         = type_init(TYPE_ARRAY);
    t->size         = base->size * size;
    t->array.base   = base;
    t->array.size   = size;
    return t;
}
Type* Ast::type_struct(SVec<TypeField*> fields){
    Type* st = type_init(TYPE_STRUCT);
    st->aggregate_items = fields;
    return st;
}
Type* Ast::type_union(SVec<TypeField*> fields){
    Type* st = type_init(TYPE_UNION);
    st->aggregate_items = fields;
    return st;
}
Type* Ast::type_proc(SVec<TypeField*> params, Type* ret_type){
    Type* t = type_init(TYPE_PROC);
    t->proc.params   = params;
    t->proc.ret_type = ret_type;
    return t;
}
Type* Ast::type_aggregate(SVec<TypeField*> items, bool isStruct){
    Type* t = type_init(isStruct? TYPE_STRUCT: TYPE_UNION);
    int size = 0;    
    t->aggregate.items = items;

    for(TypeField* item: items){
        if(isStruct){
            size += item->type->size;
        } 
        else {
            if(item->type->size > size){
                size = item->type->size;
            }
        }
    }        
    return t;
}

Type* Ast::unqualify_type(Type* type){
    if(type->kind == TYPE_CONST){
        return type->base;
    }
    return type;
}
Type* Ast::type_unresolved(){
    return &unresolved_ty;    
}
bool Ast::is_arithmetric_type(Type* type){    

    if(TYPE_FIRST_ARITHMETRIC_TYPE >= type->kind and type->kind <= TYPE_LAST_ARITHMETRIC_TYPE){
        return true;
    }    
    return false;
}

bool Ast::can_add_type(Type* lhs, Type* rhs){
    if(is_arithmetric_type(lhs) and is_arithmetric_type(rhs)){
        return true;
    }

    return false;
}


#endif /*TYPE_CPP*/