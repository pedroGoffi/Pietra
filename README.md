# Hello From Pietra Lang
- The project may change in the future
- The current target of this language is Linux with nasm assembler
# TODOS
- Selfhost
- llvm translation to another objects file

# Use declaration
- recommended to use preprocessing #package < package name >

```
#package "my_package"
// Translates to import of "std/core/linux.pi"
use  std::core{linux} 
// ~~~^~~~~~^
// std:     std is a folder
// core:    core is another folder inside std
// {linux}: is the file.pi inside std/core
```
# Procedures 
- The entry point of Pietra Lang is a procedure called main
- The main procedure c likely, can accept argc and argv
```
my_proc :: (my_parameter: parameter_type): my_ret_type {
    ...
}

main :: (){
    // To call a procedure is similar to python syntax
    my_proc(1) 
}
```

# Expression_macros
- Expression macros works like C macros without parameters
- They may be preprocessed or not
- Their usage is recommended when you don't want a magic number
#### usage: MACRO_NAME :: MACRO_EXPRESSION
```
SOME_STRING :: "Some string from macro"
std.puts(SOME_STRING)
```
# Flow Control
### If conditions , usage: if expr block
```
condition: i64 = true
if condition {
    std.puts("True\n")
}
elif condition2 {
    std.puts("True Elif 1\n")
}
else {
    std.puts("Else\n")
}
```

### While loops, usage: while expression block

```
linked_list: *ll_list = &some_linked_list
while linked_list.value {
    use(linked_list.value)
    linked_list = linked_list.next
}
```


# Data Types
In pietra lang we have some builtin types and pointers
- Integer types: i8, i16, i32, i64
- [Work in progress] floating types: f32, f64
- Pointers: **your type*
- Customize your data types with implementation declaration
```
type m_data :: i8
m_struct :: {
    m_field :: m_data
    m_ptr   :: *m_data
}

impl m_struct sayHi(self: *Self) {
    std.puts(self.m_ptr)
}

// or impl a bunch of procedures once
impl m_struct {
    decl_1 :: (self: *Self) ...
    decl_2 :: (self: *Self) ...
    decl_3 :: (self: *Self) ...
    
}
```

# stdlib
- [Linux syscalls abstraction](#linux-syscalls): std::core{linux}
- [Memory management for allocation](#memory):std{memory}
- [String methods](#string): std{string}
- [File methods](#file): std{file}
- [Pietra Configuration](#configuration): std{config}
- [Pietra math methods](#math): std{math}



# Linux syscalls
- Provides the macros for every syscall in linux
- Provides heap_begin + heap_ptr variables
- Provides struct_stat 
- Provides data types typedefinitions

Common procedures in std::core{linux}
```
use std{std}
main :: (){
    // Provided in std::core{linux} :: read
    // Provided in std::core{linux} :: write
    // Provided in std::core{linux} :: fstat
    // Provided in std::core{linux} :: brk
    // Provided in std::core{linux} :: sbrk
    // Provided in std::core{linux} :: open
    // Provided in std::core{linux} :: socket
    // Provided in std::core{linux} :: fork
    // Provided in std::core{linux} :: execve
    // Provided in std::core{linux} :: close
    // Provided in std::core{linux} :: time
    // Provided in std::core{linux} :: exit

    buff_len: i64 = 128
    buff: *i8 = std.allocate(buff_len)
    read(stdin.fd, buff, buff_len)    
    std.puts(buff)        
    exit(1)
}
```
# Memory
- Proves a struct called MEM_POLL and one method called allocate
```
use std{std}
main :: (){
    my_pointer: *i64 = std.allocate(8);
    *my_pointer = 10;
    dump(*my_pointer) // Will print 10 to stdout
}
```

# String 
- Provides a lot of methods for dealling with null terminator strings
memcpy
- C likely procedure cstrtoi
- C likely procedure itocstr
- C likely procedure streq
- C likely procedure strncmp
- C likely procedure cstrdup
- C likely procedure cstrpcpy
- C likely procedure input
- C likely procedure cstrcat
# File
- Provides FILE struct and methods C likely 

# Configuration
- Provides procedures that the compiler uses 

# Math
- Provides some simple procedures to math usage