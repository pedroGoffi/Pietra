#ifndef TYPE_CPP
#define TYPE_CPP
#include "../include/type.hpp"
#include <cstdint>
#include <cstring>
#include "arena.cpp"

using namespace Pietra;
using namespace Pietra::Ast;
using namespace Pietra::Core;

// TODO: cached types for faster compilation

namespace Pietra::Ast{


Type int8_ty        = {.kind = TYPE_I8,     .name = Core::cstr("i8"),   .size = sizeof(int8_t), .ismut = true};
Type int16_ty       = {.kind = TYPE_I16,    .name = Core::cstr("i16"),  .size = sizeof(int16_t), .ismut = true};
Type int32_ty       = {.kind = TYPE_I32,    .name = Core::cstr("i32"),  .size = sizeof(int32_t), .ismut = true};
Type int64_ty       = {.kind = TYPE_I64,    .name = Core::cstr("i64"),  .size = sizeof(int64_t), .ismut = true};
Type f32_ty         = {.kind = TYPE_F32,    .name = Core::cstr("f32"),  .size = sizeof(float)};
Type f64_ty         = {.kind = TYPE_F64,    .name = Core::cstr("f64"),  .size = sizeof(double)};
Type str_ty         = {.kind = TYPE_PTR,    .name = Core::cstr("cstr"), .size = sizeof(void*), .base = &int8_ty};
Type void_ty        = {.kind = TYPE_VOID,   .name = Core::cstr("null"), .size = 8, .ismut = false};
Type unresolved_ty  = {.kind = TYPE_UNRESOLVED, .name = Core::cstr("auto")};
Type any_ty         = {.kind = TYPE_ANY,    .name = Core::cstr("any"), .size = sizeof(void*), .ismut = true};
Type self_ty        = {.kind = TYPE_SELF,   .name = Core::cstr("Self"), .ismut = true};



TypeField* init_typefield(const char* name, Type* type){
    TypeField* tf = Core::arena_alloc<TypeField>();
    tf->name = name;
    tf->type = type;
    return tf;
}
Type* type_init(TypeKind kind, bool ismut = false){
    Type* t = Core::arena_alloc<Type>();    
    t->kind     = kind;
    t->ismut    = ismut;
    return t;
}
Type* type_void(){
  return &void_ty;
    Type* null = type_init(TYPE_VOID, false);
    null->name = Core::cstr("null");
    null->size = 0;
    return null;
}
Type* type_self(bool ismut){
  return &self_ty;
    Type* self = type_init(TYPE_SELF, ismut);
    self->name = Core::cstr("Self");
    self->size = 8;
    return self;
}
Type* type_int(int size, bool ismut){
    assert(size == 8 or size == 16 or size == 32 or size == 64);    
    TypeKind kind = TYPE_NONE;
    const char* name;

    switch(size){
        case 8:{
            return &int8_ty;
            kind = TYPE_I8;  
            name = Core::cstr("i8");
            break;
        }
        case 16:{
            return &int16_ty;
            kind = TYPE_I16;   
            name = Core::cstr("i16");
            break;
        }
        case 32:{
            return &int32_ty;
            kind = TYPE_I32;    
            name = Core::cstr("i32");
            break;
        }
        case 64:{
            return &int64_ty;
            kind = TYPE_I64;    
            name = Core::cstr("i64");
            break;
        }
    }
    Type* type = type_init(kind, ismut);
    type->ismut = ismut;
    type->size = size;
    type->name = name;
    return type;
}
Type* type_float(int size = 64, bool ismut = false){
    assert(size == 32 or size == 64);
    return size == 64
      ? &f64_ty
      : &f32_ty;
    Type* f_t = type_init(
        size == 64? TYPE_F64: TYPE_F32,
        ismut
    );
    f_t->size = 8;
    f_t->name = Core::cstr(size == 64? "f64": "f32");
    return f_t;
}
Type* type_ptr(Type* base, bool ismut){
    Type* t = type_init(TYPE_PTR, ismut);
    assert(sizeof(void*) == 8);
    t->size = sizeof(void*);
    t->base = base;
    return t;
}
Type* type_string(bool ismut){
    // str :: *imut i8        
    Type* ptr = type_ptr(type_init(TYPE_I8, false), ismut);
    ptr->name = Core::cstr("cstr");
    ptr->base->name = Core::cstr("i8");
    ptr->base->size = 1;    
    return ptr;
}
Type* type_array(Type* base, int size, bool ismut){
    Type* t         = type_init(TYPE_ARRAY, ismut);
    t->size         = base->size * size;
    t->array.base   = base;
    t->array.size   = size;
    return t;
}
Type* type_struct(SVec<TypeField*> fields, bool ismut){
    Type* st = type_init(TYPE_STRUCT, ismut);
    st->aggregate.items = fields;
    return st;
}
Type* type_union(SVec<TypeField*> fields, bool ismut){
    Type* st = type_init(TYPE_UNION, ismut);
    st->aggregate.items = fields;
    return st;
}
Type* type_proc(const char* name, SVec<TypeField*> params, Type* ret_type, bool is_vararg, bool ismut){    
    Type* t = type_init(TYPE_PROC, ismut);
    t->name = Core::cstr(name);
    t->size = 8;
    t->proc.params   = params;
    t->proc.ret_type = ret_type;
    t->proc.is_vararg = is_vararg;
    t->ismut = true;
    return t;
}
Type* type_any(){
    Type* any = type_init(TYPE_ANY, true);
    any->size = 8;
    any->name = Core::cstr("any");
    
    return any;
}
Type* type_aggregate(SVec<TypeField*> items, bool isStruct, bool ismut){
    Type* t = type_init(isStruct? TYPE_STRUCT: TYPE_UNION, ismut);
    t->ismut = ismut;
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

Type* unqualify_type(Type* type){
    if(type->kind == TYPE_CONST){
        return type->base;
    }
    return type;
}
Type* type_unresolved(){
    return type_init(TYPE_UNRESOLVED, false);
}
bool isNumericType(Type* type){    
    return  type->kind == TYPE_I8 or 
            type->kind == TYPE_I16 or 
            type->kind == TYPE_I32 or 
            type->kind == TYPE_I64 
            ;        
}

bool can_add_type(Type* lhs, Type* rhs){
    if(isNumericType(lhs) and isNumericType(rhs)){
        return true;
    }

    return false;
}

bool Type::typeCheck(Type* other){    
    if(this->kind == TYPE_ARRAY and isNumericType(other)){
        if(this->base->typeCheck(other)){
            return true;
        }
    }
    if(this->kind == TYPE_PTR and isNumericType(other)){
        return true;
    }

    if(this->kind == TYPE_ANY or other->kind == TYPE_ANY){
        return true;
    }
    if(isNumericType(this) and isNumericType(other)){
            return true;
    }

    if(this->size != other->size){
        if(this->kind == TYPE_ARRAY or other->kind == TYPE_ARRAY){
            // Array -> pointer            
            if(this->kind == TYPE_PTR or other->kind == TYPE_PTR){
                return this->base->typeCheck(other->base);                
            }
        }        
        return false;
    }

    if(this->kind == other->kind and this->kind == TYPE_PTR){
        return this->base->typeCheck(other->base);
    }
    
    return this->kind == other->kind;
}    
const char* Type::repr(){
    switch(this->kind){        
        case TYPE_VOID: return strf("null");
        
        case TYPE_I8:   return strf("i8");
        case TYPE_I16:  return strf("i16");
        case TYPE_I32:  return strf("i32");
        case TYPE_I64:  return strf("i64");
        case TYPE_F32:  return strf("i32");
        case TYPE_F64:  return strf("i64");
        case TYPE_ANY:  return strf("any");
        case TYPE_PTR: {
            const char* base = this->base->repr();
            return strf("*%s", base);
        }
        case TYPE_CONST: {
            const char* base = this->base->repr();
            return strf("const %s", base);
        }
        case TYPE_PROC: {
            const char* str = strf("%s(", this->name);
            for(TypeField* tf: this->proc.params){
                const char* str_param_t = tf->type->repr();
                str = strf("%s%s:%s", str, tf->name, str_param_t);

                if(tf != this->proc.params.back()){
                    str = strf("%s, ", str);
                }
            }

            str = strf("%s)", str);

            if(this->proc.ret_type){
                const char* str_ret_t = this->proc.ret_type->repr();
                str = strf("%s:%s", str, str_ret_t);
            }
            
            return str;
        }
        case TYPE_ARRAY:    {
            const char* str = strf("[");
            if(this->array.size > 0) {
                str = strf("%s%i", str, this->array.size);
            }
            
            str = strf("%s]", str);
            const char* base = this->array.base->repr();
            str = strf("%s%s", str, base);
            return str;
        }
        case TYPE_UNRESOLVED: return strf("<unresolved>");        

        case TYPE_STRUCT:   return strf("Struct %s", this->name);
        {        
            const char* str = strf("{ ");
            for(TypeField* tf: this->aggregate.items){

                str = strf("%s%s%s", str, tf->type->repr(), tf != this->aggregate.items.back()? ", ": "");
            }
            str = strf("%s } [AKA %s]", str, this->name);
            
            return str;

        };
        case TYPE_SELF: return strf("Self");
        
        case TYPE_UNION:                
        case TYPE_NONE:        
        case TYPE_FIRST_ARITHMETRIC_TYPE:        
        case TYPE_LAST_ARITHMETRIC_TYPE:
        default: return strf("<undefined: %i>", this->kind);
    };
    
}
bool Type::isCallable(){
    return this->kind == TYPE_PROC or this->kind == TYPE_ANY;
}
bool Type::canCast(Type* type){    
    if(type->kind == TYPE_ANY or this->kind == TYPE_ANY){
        return true;
    }
    
    if(this->calcSize() != type->calcSize()){
        return false;
    }    

    if(this->kind == TYPE_PTR or type->kind == TYPE_PTR){
        return this->kind == type->kind;
    }

    if(isNumericType(this) and isNumericType(type)){
        return true;
    }

    if(this->typeCheck(type)){
        return true;
    }

    if(this->kind != type->kind){
        return false;
    }
    

    assert(0);
}
int Type::calcSize(){
    if(this->kind == TYPE_ARRAY){
        int base_size = this->base->calcSize();
        int len_size  = 0;
        if(this->array.size > 0){
            len_size = this->array.size;
        }
        return base_size * len_size;
    }

    return this->size;
}
Type* biggestNumericType(Type* lhs, Type* rhs){
    assert(isNumericType(lhs) and isNumericType(rhs));

    return (lhs->size > rhs->size)? lhs : rhs;
}

}
#endif /*TYPE_CPP*/
