#ifndef FILE_CPP
#define FILE_CPP
#include "../include/file.hpp"
#include "arena.cpp"
#include <cstdio>
#include <fstream>
#include <string>

using namespace Pietra;

const char* fileReader::read_file(const char* filename){    
    FILE* fd = fopen(filename, "rb");
    assert(fd);
    
    fseek(fd, 0, SEEK_END);
    int fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    char* content = Core::arena_alloc<char>(fsize + 1);    
    content[fsize + 1] = '\0';
    fread(content, 1, fsize, fd);
    fclose(fd);
    return (const char*) content;

}
#endif /*FILE_CPP*/