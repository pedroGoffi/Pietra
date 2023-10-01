// NOTE: maybe i'll change this file to piAllocator.hpp
#ifndef SMALLVEC_HPP
#define SMALLVEC_HPP
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <cstdlib>

#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)
#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)
#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))

#define buf__hdr(b) ((BufHdr *)((char *)(b) - offsetof(BufHdr, buf)))
#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b) ((b) + buf_len(b))
#define buf_sizeof(b) ((b) ? buf_len(b)*sizeof(*b) : 0)
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)
#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = buf__grow((b), (n), sizeof(*(b)))))
#define buf_push(b, ...) (buf_fit((b), 1 + buf_len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))


namespace Pietra::Core {
    struct SVecHdr{
        int     len;
        int     cap;
        char    buff[];
    };
    
    template<typename T>
    struct SVec{        
        T*  data = NULL;

        SVecHdr* get_head();

    private:
        
        void __grow_buffer__(int new_len);
        void make_fit(int size);
        bool check_if_fits(int size);

    public:     
        SVec<T>() = default;    
        void push(T item);
        T    pop();
        T*   begin();
        T*   end();
        int  len();
        int  cap();

        void free();
        void reset();

        T& find(T item);        
        T& at(int index);
        T& back();
    };





    
    
    
}
#endif /*SMALLVEC_HPP*/