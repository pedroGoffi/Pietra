#ifndef PIETRA_FILE
#define PIETRA_FILE
#include "./smallVec.hpp"
#include <fstream>

namespace Pietra{
class fileReader{
public:
    static const char* read_file(const char* file);
};
}
#endif /*PIETRA_FILE*/