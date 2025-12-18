# Our-Lang V1 - Semantic Analyzer

A comprehensive semantic analyzer for **Our-Lang V1**, a simple structured programming language with Roman Urdu keywords. This project implements a complete lexical analyzer, recursive descent parser, and semantic analysis engine with data type checking and scope management.

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Language Syntax](#language-syntax)
- [Project Structure](#project-structure)
- [Compilation](#compilation)
- [Running the Analyzer](#running-the-analyzer)
- [Example Programs](#example-programs)
- [Error Checking](#error-checking)

## Overview

Our-Lang V1 is a beginner-friendly programming language designed for native Urdu speakers. It combines Roman Urdu keywords with familiar programming constructs, making it accessible while teaching programming concepts.

**Key Characteristics:**
- Dynamically typed language
- Roman Urdu keywords for intuitive learning
- Supports functions, loops, conditionals, arrays, and objects
- Entry point: `kaam main()` function
- Statements terminated with semicolons

## ✨ Features

### 1. **Lexical Analysis**
- Tokenizes source code with accurate line and column tracking
- Recognizes 11 Roman Urdu keywords
- Handles strings (single and double quotes), numbers, identifiers
- Supports comments (`//`)
- Processes operators: arithmetic, comparison, logical, assignment

### 2. **Parsing (Recursive Descent)**
- Builds Abstract Syntax Tree (AST) from tokens
- Enforces proper operator precedence
- Handles operator associativity correctly
- Supports complex nested expressions
- Parses all language constructs (functions, loops, conditionals)

### 3. **Semantic Analysis**
- **Data Type Checking**: Validates type consistency in operations
- **Scope Management**: Tracks variable and function scopes with enter/exit operations
- **Symbol Table**: Maintains declarations with initialization status
- **Error Detection**: Reports undefined variables, type mismatches, and scope violations
- **Built-in Function Validation**: Pre-defined signatures for all built-in functions

### 4. **Type System**
The analyzer recognizes and validates:
- `number` - Integer and floating-point numbers
- `string` - Text literals
- `boolean` - `haan` (true) and `na` (false)
- `array` - Collections of elements
- `object` - Key-value pairs
- `void` - Functions with no return value
- `unknown` - Uninitialized or inferred types

## Language Syntax

### Keywords
| Roman Urdu | English | Purpose |
|-----------|---------|---------|
| `banao` | declare | Variable declaration |
| `kaam` | function | Function definition |
| `agar` | if | Conditional statement |
| `warnah` | else | Else branch |
| `daura` | loop | While loop |
| `wapas` | return | Return value |
| `dekh` | print | Output to screen |
| `lou` | input | Read from user |
| `haan` | true | Boolean true |
| `na` | false | Boolean false |
| `band` | exit | Exit program |

### Operators

**Arithmetic:** `+` `-` `*` `/` `%`

**Comparison:** `==` `!=` `<` `>` `<=` `>=`

**Logical:** `&&` `||` `!`

**Assignment:** `=` `+=` `-=` `*=` `/=`

### Built-in Functions
| Function | Parameters | Returns | Description |
|----------|-----------|---------|-------------|
| `dekh()` | any | void | Print to screen |
| `lou()` | string | number | Read user input |
| `nikal()` | array/string | number | Get length |
| `abs()` | number | number | Absolute value |
| `sqrt()` | number | number | Square root |
| `pow()` | number, number | number | Power function |
| `max()` | number, number | number | Maximum value |
| `min()` | number, number | number | Minimum value |
| `round()` | number | number | Round to integer |
| `random()` | none | number | Random 0-1 |
| `band()` | none | void | Exit program |

## Project Structure

```
Our-Lang/
├── semantic_analyzer.cpp    # Main analyzer implementation (~1500 lines)
├── README.md                # This file
├── examples.txt             # Comprehensive language examples
└── test.txt                 # Test file (reads source code from here)
```

## Compilation

### Prerequisites
- C++17 compatible compiler (g++, clang, etc.)
- Linux/Unix/Mac or Windows with WSL

### Compile Command

```bash
g++ -std=c++17 -o semantic_analyzer semantic_analyzer.cpp
```

### Compilation Flags Explained
- `-std=c++17` - Use C++17 standard for modern features
- `-o semantic_analyzer` - Output executable name
- `semantic_analyzer.cpp` - Source file to compile

## Running the Analyzer

### Basic Execution

```bash
./semantic_analyzer
```

The analyzer reads source code from `test.txt` and performs analysis.

### Step-by-Step Usage

1. **Write Your Code**
   ```bash
   # Edit test.txt with your Our-Lang code
   nano test.txt
   ```

2. **Compile the Analyzer**
   ```bash
   g++ -std=c++17 -o semantic_analyzer semantic_analyzer.cpp
   ```

3. **Run Analysis**
   ```bash
   ./semantic_analyzer
   ```

4. **View Results**
   The analyzer outputs:
   - Lexical analysis results (token count)
   - Parsing results (AST generation status)
   - Semantic analysis results with any errors found

### Example Test Program

Create `test.txt`:
```
kaam square(x) {
    wapas x * x;
}

kaam main() {
    banao n = 5;
    banao result = square(n);
    dekh(result);
}
```

Run:
```bash
./semantic_analyzer
```

Output:
```
=== Our-Lang V1 Semantic Analyzer ===

Reading from: test.txt

--- Lexical Analysis ---
Tokens generated: 21

--- Parsing (Recursive Descent) ---
AST generated successfully

--- Semantic Analysis ---
- Data Type Checking
- Scope Checking
- Variable Declaration Validation
- Function Declaration Validation

✓ Semantic Analysis PASSED
```

## Example Programs

### 1. Simple Function
```
kaam add(a, b) {
    wapas a + b;
}

kaam main() {
    banao sum = add(10, 20);
    dekh(sum);
}
```

### 2. Fibonacci Sequence
```
kaam fibonacci(n) {
    banao a = 0;
    banao b = 1;
    daura (n > 0) {
        dekh(a);
        banao temp = a + b;
        a = b;
        b = temp;
        n = n - 1;
    }
}

kaam main() {
    fibonacci(10);
}
```

### 3. Prime Checker
```
kaam isPrime(num) {
    agar (num <= 1) {
        wapas na;
    }
    banao i = 2;
    daura (i * i <= num) {
        agar (num % i == 0) {
            wapas na;
        }
        i = i + 1;
    }
    wapas haan;
}

kaam main() {
    agar (isPrime(17)) {
        dekh('17 is prime');
    } warnah {
        dekh('17 is not prime');
    }
}
```

### 4. Arrays
```
kaam main() {
    banao numbers = [1, 2, 3, 4, 5];
    banao i = 0;
    daura (i < 5) {
        dekh(numbers[i]);
        i = i + 1;
    }
}
```

For more comprehensive examples, see `examples.txt`.

## Error Checking

The analyzer detects and reports:

### Type Errors
```
ERROR: Type mismatch in assignment to 'x': expected number, got string
ERROR: Right operand of '*' must be number
```

### Scope Errors
```
ERROR: Undefined variable 'x'
ERROR: Undefined function 'myFunc'
```

### Validation Errors
```
ERROR: Variable 'x' already defined in current scope
ERROR: Function 'square' expects 1 arguments, got 2
ERROR: Return statement outside function
```

### Condition Errors
```
ERROR: If condition must be boolean, got number
ERROR: Loop condition must be boolean, got string
```

## How It Works

### 1. Lexical Analysis Phase
- Reads source code character by character
- Groups characters into meaningful tokens
- Assigns token types (KEYWORD, IDENTIFIER, NUMBER, etc.)
- Tracks line and column numbers for error reporting

### 2. Parsing Phase (Recursive Descent)
- Consumes tokens from lexer
- Builds Abstract Syntax Tree following grammar rules
- Handles operator precedence with recursive function calls
- Validates syntax structure

### 3. Semantic Analysis Phase
- Traverses the AST
- Maintains symbol table with scopes
- Checks type consistency
- Validates variable initialization
- Verifies function signatures
- Reports all errors found

## Architecture

```
Source Code
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Semantic Analyzer] → Analysis Results
    ↓
✓ PASS or ✗ FAIL (with errors)
```

## Technical Details

- **Language:** C++17
- **Lines of Code:** ~1500
- **Complexity:** O(n) for lexical analysis, O(n) for parsing, O(n) for semantic analysis
- **Memory:** Dynamic allocation for AST nodes and symbol tables

## Files Included

- `semantic_analyzer.cpp` - Complete implementation
- `examples.txt` - 15+ comprehensive language examples
- `test.txt` - Input file for analysis
- `README.md` - This documentation

## Future Enhancements

- AST visualization
- Optimization suggestions
- Code generation
- Interpreter implementation
- Debugging symbols
- Performance profiling

## Contributing

This is an educational project. Feel free to extend it with:
- Additional language features
- Better error messages
- AST visualization
- Code generation
- Optimization passes

## License

Educational project for learning compiler design concepts.

---

**Created by:** Muhammad Ali Sultan, Muhammad Waleed, Hamza Safeer  
**9th Semester Project**  
**September 2025**
**Developed By:** Zeppelin Mentor
