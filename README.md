# Pietra Lang README

## Introduction

Pietra Lang is a statically-typed, compiled programming language designed to provide high performance, low-level control, and flexibility. It is primarily used for system programming and offers features like custom memory allocation, type specifications, and direct manipulation of hardware resources.

The language syntax is inspired by C-style languages but incorporates additional features like flags for customization, custom memory allocators, and specialized memory management tools. Pietra Lang is highly optimized for performance while keeping the syntax clean and expressive.

Currently, some features are still under development, including LLVM compilation, Windows portation, self-hosting, and floating types.

## Key Features

- **Low-Level Memory Management**: Pietra Lang offers direct control over memory allocation and deallocation, allowing the use of custom allocators with the `@flag("new_allocator")` directive.
  
- **Type Safety**: The language enforces strict type safety to avoid common pitfalls like type mismatches and undefined behaviors.

- **Expression Parsing**: Pietra Lang provides powerful expressions and allows custom parsers to define how expressions are evaluated.
  
- **Custom Flags**: With custom flags like `@flag("struct_reassign")` and `@flag("new_allocator")`, Pietra Lang can be extended with specific behavior that the user can define.

- **Cross-Platform Support**: The language is designed to be cross-platform, making it suitable for a variety of hardware architectures and operating systems.

---

## Getting Started

To get started with Pietra Lang, follow these steps:

1. **Install Pietra Lang**:  
   Follow the instructions in the installation guide to set up the Pietra Lang compiler and runtime environment.

2. **Create a New Project**:  
   Create a new `.pi` file, and start writing your code with the basic syntax.

3. **Compile and Run**:  
   Use the Pietra Lang compiler to compile your `.pi` files and execute them directly or create executables.

---

## Language Syntax

### Variables and Types

Pietra Lang supports several primitive types, including `i8`, `i32`, `i64`, `f32`, `f64`, and `bool`. You can define variables using the following syntax:

```pi
my_integer: i32 = 10;
my_float: f32 = 3.14;
my_boolean: bool = true;
```

### Functions and Methods

Functions are declared using the `::` operator, and methods are tied to structs and defined inside `impl` blocks. Here's an example of a function:

```pi
main :: () {
    var: i32 = 10;
    print(var);
}
```

Methods are defined within `impl` blocks. For example, the following defines a method for a custom struct:

```pi
MyStruct :: {
    value: i32
}

impl MyStruct {
    method :: (self: *Self) -> i32 {
        return self.value;
    }
}
```

### Memory Allocation

Pietra Lang allows you to allocate memory dynamically using the `new` expression. This works with the `new_allocator` flag for custom memory management.

```pi
@flag("new_allocator")
main :: () {
    // Allocate memory for 10 integers
    my_array: *i32 = new i32[10];
}
```

You can define your custom allocator like this:

```pi
@flag("new_allocator")
new_allocator :: (item_nbytes: i64, list_items: i64): *any {    
    return std.allocate(list_items * item_nbytes);
}
```

### Expression Parsing

Pietra Lang allows complex expressions, which can include operations like addition, subtraction, and logical comparisons.

Here's how a new expression is parsed:

```pi
Expr* new_expr(Lexer* lexer) {
    lexer->nextToken();
    Expr* list_items_number = nullptr;
    if(lexer->expect(TK_lbracket)){
        list_items_number = expr(lexer);
        assert(lexer->expect(TK_rbracket));
    } else {
        list_items_number = Utils::expr_int(lexer->getLocation(), 1);
    }
    TypeSpec* type = typespec(lexer);
    SVec<Expr*> args;
    if(lexer->expect(TK_lparen)) {
        args = expr_list(lexer);
        assert(lexer->expect(TK_rparen));
    }
    return Utils::expr_new(lexer->getLocation(), type, list_items_number, args);
}
```

This function handles the parsing of a `new` expression, which includes the creation of arrays and objects.

### Custom Flags

Custom flags in Pietra Lang allow users to modify or extend the behavior of the language. Flags like `@flag("struct_reassign")` or `@flag("new_allocator")` can be used to introduce specific behavior into the program's execution or compilation.

For example, the `@flag("new_allocator")` flag is used to define a custom allocator for the `new` expression, allowing developers to control memory management strategies.

```pi
@flag("struct_reassign")
struct_reassign :: (dst: *i8, src: *i8, size: i64) {
    i: i64 = 0;
    while i < size {
        dst[i] = src[i];
        i = i + 1;
    }
    return dst;
}
```

The `@flag("new_allocator")` can also be used to implement memory allocation logic based on custom requirements.

### Built-in Functions

Pietra Lang includes several built-in functions to support various tasks, including memory manipulation, type checking, and random number generation.

Here’s a list of some built-in functions:

- `std.allocate(size: i64)`: Allocates memory of the given size.
- `std.memcpy(dst: *i8, src: *i8, size: i64)`: Copies memory from `src` to `dst`.
- `std.strlen(str: *i8)`: Returns the length of the string `str`.
- `std.rand()`: Generates a random integer.

### Random Number Generation

Pietra Lang also includes support for random number generation:

```pi
RAND_MAX :: 32767;

impl std rand() -> i64 {
    return (random_next = std.rand_from(random_next));
}
```

This function can be used to generate random numbers, either for general use or within specified ranges:

```pi
random_range :: (begin: i64, end: i64) {
    return std.rand() % (end + 1 - begin) + begin;
}
```

---

## Not Yet Implemented Features

### LLVM Compilation

The ability to compile Pietra Lang code to LLVM intermediate representation (IR) is **not yet implemented**. This feature would enable developers to optimize and compile Pietra Lang code using LLVM backends for various architectures. This feature is currently under development and will be available in future versions.

### Windows Portation

The current version of Pietra Lang primarily supports Linux-based environments. A **Windows portation** is planned but not yet available. Future releases will aim to support Windows as a target platform for Pietra Lang development.

### Self-Hosting

Self-hosting, or the ability to compile the Pietra Lang compiler itself using Pietra Lang code, is **not yet implemented**. This feature is crucial for making Pietra Lang a fully self-sustained language, and it is on the roadmap for future releases.

### Floating Point Types

Support for floating point types like `f32` and `f64` is currently **under development**. The current implementation of Pietra Lang supports basic integer types, but floating-point operations, such as addition, subtraction, multiplication, and division with `f32` and `f64` types, are expected to be introduced in the future.

---

## Advanced Usage

### String Interning

Pietra Lang supports string interning for efficient string comparisons and memory usage. The `intern_range` function allows for string interning by storing unique strings in a list:

```pi
intern_range :: (begin: cstr, end: cstr): *i8 {
    node: mut *ll_node = intern_strings.head;
    while node.value {
        if std.cstrncmp(begin, node.value, end - begin) == 0 {
            return node.value;
        }
        node = node.next;
    }
    string: *i8 = std.allocate(end - begin + 1);
    std.memcpy(string, begin, end - begin);
    string[end - begin + 1] = 0;
    intern_strings.push(string);
    return string;
}
```

This improves the efficiency of string handling in Pietra Lang.

---

## Conclusion

Pietra Lang provides a powerful combination of low-level control, performance optimization, and flexible features. With its custom memory allocators, type-safe design, and flexible expression parsing, Pietra Lang is a robust choice for system-level programming tasks.

Although some features like LLVM compilation, Windows portation, self-hosting, and floating point types are **not yet implemented**, they are actively being developed and will be available in future versions of the language.

By using flags like `new_allocator` and advanced features like memory manipulation and random number generation, developers can tailor their programs to meet specific needs, optimizing performance and resource usage.