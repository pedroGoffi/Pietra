#ifndef ARENA_CPP
#define ARENA_CPP
#include "../include/arena.hpp"
#include "cGen.cpp"
#include <cstddef>
#include <cstdlib>

using namespace Pietra;

SVec<Arena<void*>*> __arenas;
Core::Arena<void*>  main_arena;



template<typename T>
Core::Arena<T>::Arena(){    
    __arenas.push(reinterpret_cast<Arena<void*>*>(this));
}

template<typename T> 
void Core::Arena<T>::grow(int min_size){    
    size_t size = ALIGN_UP(CLAMP_MIN(min_size, ARENA_BLOCK_SIZE), ARENA_ALIGNMENT);    
    this->ptr = malloc(size);    
    if(!this->ptr){
        printf("[INTERNAL-ERROR]: Failed to allocate in the arena.\n");
        exit(1);
    }
    assert(this->ptr == ALIGN_DOWN_PTR(this->ptr, ARENA_ALIGNMENT));
    this->blocks_end = (void*)((char*)this->ptr + size);

    this->push_block(this->ptr);    
}
template<typename T> 
T* Core::Arena<T>::alloc(int size){    
    if(size > (char*)this->blocks_end - (char*)this->ptr){        
        this->grow(size);
        assert(size <= (char*)this->blocks_end - (char*)this->ptr);
    }

    T* ptr = (T*)this->ptr;
    this->ptr = (char*) ALIGN_UP_PTR((char*)this->ptr + size, ARENA_ALIGNMENT);
    assert(this->ptr <= this->blocks_end);
    assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));    
    return ptr;
}
template<typename T>
void Core::Arena<T>::push_block(void* block){
    this->blocks.push(block);
}
template<typename T>
void Core::Arena<T>::free(){    
    for(void** block = this->blocks.begin(); block != this->blocks.end(); block++){        
        if(block and *block)  std::free(*block);
    }
    
}

template<typename T> T* Core::Arena<T>::begin(){
    return this->blocks_begin;
}
template<typename T> T* Core::Arena<T>::end(){
    return this->blocks_end;
}

/// GENERICAL ARENA USAGE
template<typename T>
T* Core::arena_alloc(int rep){
    T* ptr = (T*) main_arena.alloc(sizeof(T) * rep);        
    return ptr;
}

void Core::arena_free(){
    for(Arena<void*>* arena: __arenas){
        assert(arena);
        arena->free();    
    }    
}



#endif /*ARENA_CPP*/