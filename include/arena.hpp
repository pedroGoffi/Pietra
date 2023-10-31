#ifndef ARENA_HPP
#define ARENA_HPP
#include "smallVec.hpp" 
#include <cstddef>

namespace Pietra::Core {
    template<typename T> class Arena {
        static constexpr int ARENA_ALIGNMENT  = 8;
        static constexpr int ARENA_BLOCK_SIZE = 1024 * 1024;
        
        void*   ptr;
        
        void*   blocks_end;
        void*   blocks_begin;
        SVec<void*>  blocks;


    
    void grow(int min_size);
    void push_block(void* block);
    public:
        Arena<T>();
        T* alloc(int size);
        void free();
        T*   begin();
        T*   end();
    };


    /* Allocates the size of the type in the template */
    template<typename T> T* arena_alloc(int rep = 1);
    void arena_free();
    void arena_string(const char* str);    



    template<typename T> T* arena_alloc(int rep);
    void arenas_free();
}
#endif /*ARENA_HPP*/