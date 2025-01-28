# **Pietra Lang Documentation**

## Introduction

**Pietra Lang** is a programming language designed to handle both compile-time and runtime operations efficiently. It features a clear, concise syntax that supports basic constructs, variable declarations, functions, control flow, preprocessing, and built-in functions.

---

## **Table of Contents**

1. **Getting Started**
   - Installation
   - Running Pietra Code

2. **Syntax Overview**
   - Variables and Constants
   - Functions
   - Control Flow
   - Arrays and Collections

3. **Preprocessing**
   - Built-in Preprocessing Functions
   - Preprocessing Constants and Values
   - Comptime Code

4. **Built-in Functions**
   - `print`
   - `make_array`
   - `str`
   - `int`
   - `buffer_append`
   - `buffer_clear`
   - `buffer_size`
   - `buffer_get`

5. **Advanced Concepts**
   - Macros and Constants
   - Error Handling
   - Performance Optimizations

6. **Examples**
   - Basic Example
   - Working with Arrays
   - Implementing Factorial

7. **Package System**
   - Creating and Using Packages
   - Importing External Packages

---

## **1. Getting Started**

### Installation

To use Pietra Lang, you'll need to install the language runtime and compiler. The installation process varies by platform.

#### Example:
1. Clone the Pietra Lang repository.
2. Build the compiler using the provided makefile.
3. Install any required dependencies.

### Running Pietra Code

You can run Pietra code by specifying the path to your `.pi` file:
```bash
pietra run path_to_file.pi
```

---

## **2. Syntax Overview**

### Variables and Constants

In Pietra Lang, variables are declared using the `:=` operator:
```pi
myvar := 10
```
Variables are dynamically typed, meaning the type is inferred from the assigned value.

To define constants, use the `const` keyword:
```pi
const PI := 3.14159
```

### Functions

Functions in Pietra Lang are declared using the following syntax:
```pi
function_name :: (param: type): return_type {
    // Function body
}
```

Example of a simple function:
```pi
add :: (a: i64, b: i64): i64 {
    return a + b
}
```

You can also use a function that doesn’t return anything by using `None` as the return type.

### Control Flow

#### Conditional Statements

Use the `if` statement for conditionals:
```pi
if condition {
    // Code block
}
```

Example:
```pi
if x > 10 {
    print("x is greater than 10")
}
```

#### Loops

Pietra Lang supports `while` loops for iteration:
```pi
while condition {
    // Code block
}
```

Example:
```pi
i := 0
while i < 5 {
    print(i)
    i = i + 1
}
```

### Arrays and Collections

Arrays are created using `make_array` and elements can be added using the `.push()` method:
```pi
arr := make_array()
arr.push("Hello")
arr.push("World")
```

To retrieve the array’s string representation, use the `.repr()` method:
```pi
arr_repr := arr.repr()
```

To access the last element, use `.end()`:
```pi
last_elem := arr.end()
```

---

## **3. Preprocessing**

### Built-in Preprocessing fields
- is_error check if a result is error 
- is_none  check if a result is none
- error    get the error message, if no error it will be str(result)
- **Observation**: those fields are not allocated but in preprocessing visit node method checked

Example:
```pi
f := factorial(5)
if f.is_none {
    print("!F is none")
}
if f.is_error {
    print("[FACTORIAL ERROR]: ", f.error)
}

print(f)
```

### Macros

You can be defined using macros:
```pi
MACRONAME :: MACROVALUE
```

### Comptime Code

Code that should run during compilation (comptime) can be annotated with a comment:
```pi
// NOTE: This code will only run at compile-time
```

You can also define compile-time behavior using special constructs or functions.

---

## **4. Built-in Functions in preprocess stage**

Pietra Lang comes with several built-in functions for handling basic operations.

- **`print`**: Prints the values passed to it.
  ```pi
  print("Hello, world")
  ```

- **`make_array`**: Creates a new empty array.
  ```pi
  arr := make_array(optional_args)
  ```

- **`str`**: Converts a value to a string.
  ```pi
  str_val := str(123)  // "123"
  ```

- **`int`**: Converts a value to an integer.
  ```pi
  int_val := int("123")  // 123
  ```

- **`buffer_append`**: Appends data to a buffer.
  ```pi
  buffer_append(buf, "data")
  ```

- **`buffer_clear`**: Clears the buffer.
  ```pi
  buffer_clear(buf)
  ```

- **`buffer_size`**: Returns the size of the buffer.
  ```pi
  buf_size := buffer_size(buf)
  ```

- **`buffer_get`**: Retrieves data from the buffer.
  ```pi
  data := buffer_get(buf)
  ```

---

## **5. Advanced Concepts**

### Macros and Constants

Macros in Pietra Lang are like constants, but they are expanded at compile-time:
```pi
MACRO_X :: 100
```

These macros allow you to create reusable values or pieces of code that are substituted during compilation.

### Error Handling in preprocessing stage

Pietra Lang does not have traditional exceptions but handles errors using return values like `None`:
```pi
if result.is_none {
    print("An error occurred.")
}
```

### Performance Optimizations

Preprocessing can also help optimize certain parts of your code during the compile-time, reducing the runtime overhead. Macros and constant folding are two examples of compile-time optimizations.

---

## **6. Examples**

### Basic Example

```pi
main :: () {
    x := 10
    y := 20
    result := add(x, y)
    print("Result of addition:", result)
}
```

### Working with Arrays

```pi
main :: () {
    arr := make_array()
    arr.push("Hello")
    arr.push("World")
    print(arr.repr())  // Outputs: ['Hello', 'World']
}
```

### Implementing Factorial

```pi
factorial :: (x: i64): i64 {
    result := 1
    i:= 1
    while i < x + 1{
        result := result * i
        i = i + 1
    }
    return result
}

main :: () {
    fact := factorial(5)
    print("Factorial of 5 is", fact)  // Outputs: 120
}
```

---

## **7. Package System**

### Creating and Using Packages

Pietra Lang supports creating packages and importing them into your main code. To create a package, place your `.pi` files in a specific directory. Then, import them into your code using:
```pi
#package "package_name"
```

### Importing External Packages

To use an external package, you can import it in the following way:
```pi
#run "package path"
```

---

## **Conclusion**

Pietra Lang is designed to be simple yet powerful, offering flexibility in both runtime and compile-time processing. With support for macros, preprocessing, and built-in functions, it allows developers to write efficient and readable code. The language is particularly well-suited for scenarios where performance optimizations and compile-time evaluations are crucial.

