traction via dedicated standard headers, dynamic include-path lookup algorithms, and scalable Makefile build automation.

## 2. Complete File Directory & Structural Map

```
mypreprocessor/
├── Makefile                         # Optimized GNU Make build configuration
├── mypreprocessor                   # Compiled executable binary (ELF target)
├── mypreprocessor_viva_Que.pdf      # Oral examination & technical interview question bank
│
├── include/                         # Header files (public definitions & APIs)
│   ├── comments.h                   # State machine interface for comment stripping
│   ├── common.h                     # Global macros, data structures, and constants
│   ├── expand.h                     # Text expansion and #ifdef logic evaluation engine
│   ├── includes.h                   # Header search path management & include resolver
│   ├── macro.h                      # Key-value symbol table interface for macro definitions
│   ├── preprocessor.h               # Core orchestration engine and execution pipeline
│   └── utils.h                      # Safe memory allocation and string utility headers
│
├── src/                             # Source modules and compiled object files
│   ├── comments.c                   # Implements DFA comment stripper logic
│   ├── comments.o                   # Compiled object code for comments subsystem
│   ├── expand.c                     # Text replacement and conditional evaluation logic
│   ├── expand.o                     # Compiled object code for text expansion
│   ├── includes.c                   # Path search and recursive header resolution
│   ├── includes.o                   # Compiled object code for header resolution
│   ├── macro.c                      # Dynamic symbol table tracking (#define / #undef)
│   ├── macro.o                      # Compiled object code for symbol table manager
│   ├── main.c                       # Command line option parsing and CLI entry point
│   ├── main.o                       # Compiled object code for main CLI module
│   ├── preprocessor.c               # Master stream loop and pipeline driver
│   ├── preprocessor.o               # Compiled object code for pipeline orchestration
│   ├── utils.c                      # Memory checks, tokenizers, and string helper code
│   └── utils.o                      # Compiled object code for utility functions
│
├── sysheaders/                      # Built-in system header simulation folder
│   ├── mypredef.h                   # System predefined macro signatures
│   └── toolkit.h                    # General utility headers for internal mock testing
│
└── tests/                           # Complete test suite, mock headers, and output targets (.i)
    ├── acceptance.c                 # End-to-end integration test file
    ├── acceptance.i                 # Preprocessed reference output for acceptance test
    ├── all_points.c                 # Full directive coverage test file
    ├── all_points.h                 # Test header for all_points execution
    ├── all_points.i                 # Processed output of all_points.c
    ├── demo.c                       # Basic demonstration source code
    ├── demo.h                       # Basic demonstration header code
    ├── full_test.c                  # Comprehensive multi-header integration test file
    ├── full_test.i                  # Reference preprocessed output of full_test.c
    ├── helper.h                     # Sub-header target for nested includes
    ├── mathx.h                      # Test header for macro-based mathematical functions
    ├── predef_test.c                # Test for built-in system macros (__LINE__, __FILE__)
    ├── predef_test.i                # Expanded output of predef_test.c
    ├── t1_comments.c                # Unit test for single and multi-line comments
    ├── t1_comments.i                # Preprocessed output showing comment removal
    ├── t2_objmacro.c                # Unit test for object-like macro definitions
    └── t2_objmacro.i                # Preprocessed output showing expanded object macros

```

## 3. Subsystem Architecture & Implementation Details

### 3.1. Core Orchestration Engine

* **Files:** `src/main.c`, `src/preprocessor.c`, `include/preprocessor.h`

* **Responsibilities:**

  * Parses command-line arguments using standard options (`-I` for search directories, `-o` for custom output file paths, and `-v` for verbose diagnostic tracing).

  * Manages input stream opening and output target generation (`.i` extension).

  * Executes the multi-stage pipeline line by line:
    

    $$
    \text{Raw Source} \longrightarrow \text{Comment Removal} \longrightarrow \text{Directive Parsing} \longrightarrow \text{Macro Expansion} \longrightarrow \text{Output File}
    $$

### 3.2. Lexical Comment Stripper

* **Files:** `src/comments.c`, `include/comments.h`

* **Mechanism:**

  * Employs a Deterministic Finite Automaton (DFA) state machine:

    * `STATE_NORMAL`: Writes characters directly to the output buffer.

    * `STATE_SINGLE_COMMENT`: Triggered on `//`. Discards text until `\n` is reached.

    * `STATE_MULTI_COMMENT`: Triggered on `/*`. Discards text until `*/` sequence is closed.

    * `STATE_STRING_LITERAL`: Ignores comment syntax nested within double quotes (`"..."`).

  * Replaces comments while preserving newlines (`\n`), ensuring line numbers in error messages stay accurate to the original input.

### 3.3. Include Engine & Directory Traversal

* **Files:** `src/includes.c`, `include/includes.h`, `sysheaders/`

* **Responsibilities:**

  * Parses `#include "file.h"` (local relative lookup) and `#include <file.h>` (system search path lookup).

  * Iterates dynamically over an internal list of directories supplied via `-I` command-line flags.

  * Recursively opens headers and pipes content back through the preprocessor pipeline.

  * Tracks an inclusion stack to prevent infinite recursion caused by circular `#include` paths.

### 3.4. Macro Symbol Table Subsystem

* **Files:** `src/macro.c`, `include/macro.h`

* **Data Structures & Logic:**

  * Operates a dynamic key-value lookup table (dynamic list or hash-indexed linked nodes).

  * Processes `#define MACRO_NAME replacement_value` to register new entries.

  * Processes `#undef MACRO_NAME` to erase entries from the symbol table.

  * Stores both object-like macros (`#define MAX_BUFFER 1024`) and provides foundational support for function-like macro patterns.

### 3.5. Text Expansion & Conditional Compilation Engine

* **Files:** `src/expand.c`, `include/expand.h`

* **Responsibilities:**

  * Evaluates conditional control block directives: `#ifdef`, `#ifndef`, `#else`, and `#endif`.

  * Maintains an internal stack of boolean flags representing conditional output states. When a branch evaluates to false, emission to the output stream is suspended.

  * Performs tokenization on active lines, checking tokens against the symbol table and expanding matching identifiers with their stored value.

### 3.6. Utility Subsystem

* **Files:** `src/utils.c`, `include/utils.h`, `include/common.h`

* **Responsibilities:**

  * Encapsulates system allocation calls (`malloc`, `realloc`, `calloc`) with built-in null-pointer checks to prevent memory exhaustion crashes.

  * Performs string normalization, leading/trailing whitespace trimming, string concatenation, and token boundary verification.

## 4. Build System & Compilation Directives

### 4.1. Makefile Rules

The provided `Makefile` manages incremental compilation using the GNU C Compiler (`gcc`):

* **Default Compiler Flags:**

  ```
  CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude
  
  ```

  * `-Wall -Wextra`: Enforces strict compile-time warning diagnostics.

  * `-O2`: Applies level-2 code optimizations.

  * `-std=c11`: Targets the ISO/IEC 9899:2011 C standard.

  * `-Iinclude`: Automatically points header paths to the `include/` directory.

* **Primary Make Targets:**

  * `make` / `make all`: Compiles source dependencies under `src/` into object files (`.o`) and links them into the `./mypreprocessor` executable binary.

  * `make clean`: Removes binary executables, object files (`*.o`), and generated output targets (`*.i`).

  * `make test`: Executes automated preprocessor testing across inputs in `tests/`.

## 5. Usage & CLI Manual

### 5.1. Command Syntax

```
./mypreprocessor <input.c> [-I<dir>]... [-o output.i] [-v]

```

### 5.2. Options & Parameters

| Option / Parameter | Type | Description | 
| ----- | ----- | ----- | 
| `<input.c>` | Required | Path to the target C source code file to preprocess. | 
| `-I<dir>` | Optional | Add directory `<dir>` to the include file search path (can be used multiple times). | 
| `-o output.i` | Optional | Specifies the path for the preprocessed output file. Defaults to `stdout` if omitted. | 
| `-v` | Optional | Activates verbose execution mode, outputting debug steps to `stdout`. | 

### 5.3. Example Usage

```
# Basic run generating demo output
./mypreprocessor tests/demo.c -o tests/demo.i

# Preprocessing with custom include paths and verbose debugging
./mypreprocessor tests/full_test.c -I./sysheaders -I./tests -o tests/full_test.i -v

```

## 6. Test Suite Matrix

The `tests/` directory contains tests designed to validate individual sub-components and overall system integration:

| Test File | Primary Objective | Description | Expected Output Target | 
| ----- | ----- | ----- | ----- | 
| `t1_comments.c` | Comment Engine | Validates removal of both `//` and `/* ... */` comment formats without disrupting line numbering. | `t1_comments.i` | 
| `t2_objmacro.c` | Macro Engine | Validates registration, lookup, and text substitution of `#define` macros. | `t2_objmacro.i` | 
| `predef_test.c` | Environment Directives | Verifies expansion of predefined system identifiers (`__LINE__`, `__FILE__`). | `predef_test.i` | 
| `all_points.c` | Directive Parser | Tests complete conditional workflow (`#ifdef`, `#ifndef`, `#else`, `#undef`, `#endif`). | `all_points.i` | 
| `acceptance.c` | Integration Verification | Full pipeline test combining comments, inclusions, macro expansion, and conditionals. | `acceptance.i` | 
| `full_test.c` | Stress & Recursion | Complex test executing multi-level nested headers (`helper.h`, `mathx.h`). | `full_test.i` | 

## 7. Viva & Oral Defense Preparation Guide (`mypreprocessor_viva_Que.pdf`)

The root repository includes `mypreprocessor_viva_Que.pdf`, a reference document for academic project vivas or technical architecture reviews. Key topics covered include:

1. **Tokenization vs. String Replacement:** Why simple string substitution (`strstr`) fails without token boundary checks (e.g., preventing macro `CAT` from modifying variable `CATERPILLAR`).

2. **State Machine Design:** Managing edge cases during comment stripping, such as comment markers enclosed in string literals (`char *s = "/* not a comment */"`).

3. **Data Structure Trade-Offs:** Comparing dynamic linked lists ($O(N)$ lookup) to Hash Tables ($O(1)$ average lookup) for symbol table management.

4. **Recursive File Inclusion:** Preventing infinite loops with circular includes using path-tracking stacks.

5. **Memory Safety:** Preventing memory leaks during string reallocation when expanding short macro names into longer values.

## 8. License

Distributed under the **MIT License**. Refer to the repository root or standard open-source guidelines for permissions and terms.# Comprehensive Technical Specification & Documentation: `mypreprocessor`

## 1. Executive Overview

`mypreprocessor` is a lightweight, modular, C11-compliant systems utility written in standard C from scratch to replicate core features of the standard GNU C Preprocessor (`cpp`). The tool processes standard C source code prior to compilation by executing lexical transformations, recursive file inclusions, conditional block evaluations, comment stripping via deterministic finite automata, and dynamic macro symbol-table tracking.

Designed with modular systems architecture in mind, the codebase adheres to strict standards of memory hygiene, clean interface abs
