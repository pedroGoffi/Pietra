#ifndef INTERN_HPP
#define INTERN_HPP
#include "arena.hpp"
namespace Pietra::Core {
    struct Intern_string {
        const char* data;
        size_t      len;
    };

    const char* cstr_range(const char* begin, const char* end);
    const char* cstr(const char* str);

}
#endif /*INTERN_HPP*/