#ifndef ASMPLANG
#define ASMPLANG
#include "../include/Asmx86_64.hpp"
#include "resolve.cpp"
#include "ast.cpp"
#include <cmath>
#include <cstdio>
#include <map>

#define DEBUG_MODE      false
#define __OUT DEBUG_MODE? stdout: OUT

#define printf(...) fprintf(__OUT, __VA_ARGS__)
#define err(...) fprintf(stderr, __VA_ARGS__)
#define printc(...) printf("\t"); printf(__VA_ARGS__)

using namespace Pietra;
using namespace Pietra::Resolver;
using namespace Ast;
using PType = Ast::Type;
std::map<const char*, Ast::Expr*> constexpr_map;
const char* argreg8[]  = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
const char *argreg16[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
const char *argreg32[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
#define MAX_ARGREG 6

const char* BUILTIN_SYSCALL_NAME = Core::cstr("__syscall");
const char* BUILTIN_PRINTI = 
    "__print:\n"
    "\tpush  rbp\n"
    "\tmov   rbp, rsp\n"
    "\tsub   rsp, 64\n"
    "\tmov   QWORD  [rbp-56], rdi\n"
    "\tmov   QWORD  [rbp-8], 1\n"
    "\tmov   eax, 32\n"
    "\tsub   rax, QWORD  [rbp-8]\n"
    "\tmov   BYTE  [rbp-48+rax], 10\n"
    ".L2:\n"
    "\tmov   rcx, QWORD  [rbp-56]\n"
    "\tmov   rdx, -3689348814741910323\n"
    "\tmov   rax, rcx\n"
    "\tmul   rdx\n"
    "\tshr   rdx, 3\n"
    "\tmov   rax, rdx\n"
    "\tsal   rax, 2\n"
    "\tadd   rax, rdx\n"
    "\tadd   rax, rax\n"
    "\tsub   rcx, rax\n"
    "\tmov   rdx, rcx\n"
    "\tmov   eax, edx\n"
    "\tlea   edx, [rax+48]\n"
    "\tmov   eax, 31\n"
    "\tsub   rax, QWORD  [rbp-8]\n"
    "\tmov   BYTE  [rbp-48+rax], dl\n"
    "\tadd   QWORD  [rbp-8], 1\n"
    "\tmov   rax, QWORD  [rbp-56]\n"
    "\tmov   rdx, -3689348814741910323\n"
    "\tmul   rdx\n"
    "\tmov   rax, rdx\n"
    "\tshr   rax, 3\n"
    "\tmov   QWORD  [rbp-56], rax\n"
    "\tcmp   QWORD  [rbp-56], 0\n"
    "\tjne   .L2\n"
    "\tmov   eax, 32\n"
    "\tsub   rax, QWORD  [rbp-8]\n"
    "\tlea   rdx, [rbp-48]\n"
    "\tlea   rcx, [rdx+rax]\n"
    "\tmov   rax, QWORD  [rbp-8]\n"
    "\tmov   rdx, rax\n"
    "\tmov   rsi, rcx\n"
    "\tmov   edi, 1\n"
    "\tmov   rax, 1\n"
    "\tsyscall\n"
    "\tnop\n"
    "\tleave\n"
    "\tret\n"
    ;


int count(){
    static long int __label_count = 0;
    return __label_count++;
}

FILE* OUT;
Asm::Compiled_procs     Cps;    // Compiled procs
Asm::Compiled_proc*     Cp;     // Current proc
Asm::Compiled_strings   Str;    // Strings
Asm::Compiled_vars      GVars;  // Global vars
Asm::Compiled_vars      LVars;  // Local vars
Asm::Compiled_context   ACtx;   // Assembly context




#undef printf
#endif /*ASMPLANG*/