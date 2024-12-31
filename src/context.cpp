#ifndef CONTEXT_CPP
#define CONTEXT_CPP
#include "bridge.cpp"
using namespace CBridge;


// Global instances
Context::AssemblyContext                ctx;
TypeDefinitions::TypeDefinitionManager  typedefs;
SVec<const char*> global_strings;
// Compiler states

#endif /*CONTEXT_CPP*/