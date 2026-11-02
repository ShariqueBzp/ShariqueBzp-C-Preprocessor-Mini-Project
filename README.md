# Comprehensive Technical Specification & Project Manual: `mypreprocessor`

## 1. Project Background & Overview

`mypreprocessor` is a modular, C11-compliant systems-programming utility written from scratch in standard C. It is designed to emulate the fundamental pipeline and processing mechanisms of the standard **GNU C Preprocessor (`cpp`)**.

Before a C compiler (`gcc` or `clang`) translates source code into assembly or machine code, the preprocessor transforms raw text files by stripping comments, executing conditional branching, dynamically replacing macro identifiers, and recursively replacing `#include` directives with the raw contents of header files. `mypreprocessor` provides a clean, fully customized implementation of this preprocessing pipeline.

### Core Objectives

* **Modular Codebase Architecture:** Clean separation of concerns across header definitions (`include/`), implementation modules (`src/`), system-level headers (`sysheaders/`), and test suites (`tests/`).
* **Lexical Comment Stripping:** Neutralize single-line (`//`) and multi-line (`/* ... */`) comments via a Deterministic Finite Automaton (DFA) state machine while maintaining strict line-number parity.
* **Recursive File Inclusion:** Intercept and expand both `#include "file.h"` and `#include <file.h>` directives, including recursive inclusion handling and custom include directory search paths (`-I`).
* **Dynamic Macro Symbol Table Management:** Parse and manage macro definitions (`#define`) and dynamic undefinitions (`#undef`) through in-memory symbol tables.
* **Conditional Compilation:** Evaluate conditional control blocks (`#ifdef`, `#ifndef`, `#else`, `#endif`) to dynamically include or suppress emitting code blocks.
* **Text & Identifier Expansion:** Perform word-boundary-aware token scanning to substitute defined macro keys with their assigned expansion strings without corrupting string literals or partial variable name matches.
* **Predefined Environment Macros:** Expand built-in preprocessor macros like `__LINE__` and `__FILE__`.

## 2. Comprehensive Directory & File Structure

```text
mypreprocessor/
├── Makefile                         # GNU Make build script with optimized compiler flags
├── mypreprocessor                   # Compiled executable binary target (ELF format)
├── mypreprocessor_viva_Que.pdf      # Question bank & technical guide for oral viva defense
│
├── include/                         # Header files (public interface declarations)
│   ├── comments.h                   # DFA state machine signatures for comment stripping
│   ├── common.h                     # Global constants, data types, and system configurations
│   ├── expand.h                     # Text macro replacement and #ifdef evaluation interfaces
│   ├── includes.h                   # Search path resolver and header inclusion module API
│   ├── macro.h                      # Symbol table structures and key-value macro manager API
│   ├── preprocessor.h               # Core execution pipeline driver & stream controller API
│   └── utils.h                      # Memory safety wrappers and string utility headers
│
├── src/                             # Source implementation modules
│   ├── comments.c                   # Implementation of lexical comment removal logic
│   ├── expand.c                     # Logic for text macro expansion & conditional branching
│   ├── includes.c                   # Implementation of include path search & recursion
│   ├── macro.c                      # Data structure implementation for macro symbol table
│   ├── main.c                       # CLI argument handling, option parsing, and main entry point
│   ├── preprocessor.c               # Main line-by-line streaming & processing pipeline
│   └── utils.c                      # Utility functions: dynamic memory wrappers, string trimming
│
├── sysheaders/                      # Standard and system header simulation directory
│   ├── mypredef.h                   # Mock system predefined macros and guards
│   └── toolkit.h                    # Simulated system toolkit utility header
│
└── tests/                           # Integrated test suite source inputs
    ├── acceptance.c                 # End-to-end integration test source file
    ├── all_points.c                 # Comprehensive test covering all directive features
    ├── all_points.h                 # Supporting header for all_points.c
    ├── demo.c                       # Simple demonstration test source file
    ├── demo.h                       # Supporting header for demo.c
    ├── full_test.c                  # Advanced integration test with multi-level nested includes
    ├── helper.h                     # Sub-header target for nested inclusion testing
    ├── mathx.h                      # Test header containing mathematical macro definitions
    ├── predef_test.c                # Test targeting built-in environment macros (__LINE__, __FILE__)
    ├── t1_comments.c                # Focused test for single-line and multi-line comments
    └── t2_objmacro.c                # Focused test for object-like macro definitions
```

## 3. Detailed Subsystem Architecture & Implementation

### 3.1. Main Execution Engine & Pipeline Orchestration

* **Files:** `src/main.c`, `src/preprocessor.c`, `include/preprocessor.h`
* **Workflow:**
  1. `main.c` parses command-line parameters (inputs, `-I` search directories, `-o` output targets, and `-v` verbose flags).
  2. The input stream is opened, and an output file or `stdout` stream is prepared.
  3. The processing loop reads the source file line by line, passing each string through sequential pipeline transformations:

     $$ \text{Raw Line} \xrightarrow{\text{Comments DFA}} \text{Comment-Free Line} \xrightarrow{\text{Directive Parser}} \text{Evaluated Line} \xrightarrow{\text{Expand Engine}} \text{Final Output Line} $$

### 3.2. Lexical Comment Stripper

* **Files:** `src/comments.c`, `include/comments.h`
* **Implementation:** Uses a **Deterministic Finite Automaton (DFA)** to inspect characters while maintaining state:
  * `STATE_NORMAL`: Passes standard code characters directly to the output buffer.
  * `STATE_SINGLE_COMMENT`: Activated by `//`. Discards all following characters until a newline (`\n`) is reached.
  * `STATE_MULTI_COMMENT`: Activated by `/*`. Discards characters until `*/` is encountered.
  * `STATE_STRING_LITERAL`: Activated inside double quotes (`"..."`). Ignores `//` or `/*` inside strings so valid text like `"http://example.com"` remains untouched.
* **Line Number Preservation:** Replaces multi-line comment blocks with equivalent newline sequences so line count tracking remains completely intact.

### 3.3. Include Resolver & Directory Search Engine

* **Files:** `src/includes.c`, `include/includes.h`, `sysheaders/`
* **Implementation:**
  * Handles local inclusions `#include "filename.h"` (searches current directory first, then `-I` paths).
  * Handles system inclusions `#include <filename.h>` (searches `-I` paths and system header paths like `sysheaders/`).
  * Manages an active inclusion stack to prevent infinite recursion or circular dependency freezes.

### 3.4. Macro Symbol Table Subsystem

* **Files:** `src/macro.c`, `include/macro.h`
* **Implementation:**
  * Maintains an in-memory dynamic key-value symbol table storing defined macro names and their replacement bodies.
  * Registers new entries upon encountering `#define MACRO_NAME replacement`.
  * Purges existing entries upon encountering `#undef MACRO_NAME`.
  * Provides key lookup functions used by both the conditional evaluator and text expansion engines.

### 3.5. Expansion Engine & Conditional Compilation

* **Files:** `src/expand.c`, `include/expand.h`
* **Implementation:**
  * Evaluates block directives: `#ifdef MACRO`, `#ifndef MACRO`, `#else`, and `#endif`.
  * Uses a stack-based conditional state machine to suppress or allow line output based on current block condition truth values.
  * Scans active code lines for identifier tokens matching symbol table keys, substituting them with their expanded values while respecting word boundaries.

### 3.6. Utility Helpers & Memory Safety

* **Files:** `src/utils.c`, `include/utils.h`, `include/common.h`
* **Implementation:**
  * Wraps memory allocations (`malloc`, `calloc`, `realloc`) to ensure strict check-on-allocation hygiene and prevent crashes or memory leaks.
  * Provides helper routines for string trimming, token boundary identification, and path formatting.

## 4. Build Directives & Compilation

The project uses GNU Make for compilation.

### Compiler Configuration

* **Compiler Flags (`CFLAGS`):** `-Wall -Wextra -O2 -std=c11 -Iinclude`
  * `-Wall -Wextra`: Enables full warning diagnostics.
  * `-O2`: Enables level 2 code optimization.
  * `-std=c11`: Compiles under the ISO C11 language specification standard.
  * `-Iinclude`: Automatically adds the project header directory to the include path.

### Key Make Commands

```bash
# Build the project executable (mypreprocessor)
make

# Clean up generated object files (*.o) and preprocessed output files (*.i)
make clean

# Run standard test executions against test suite inputs
make test
```

## 5. Command-Line Interface (CLI) Manual

### Command Syntax

```bash
./mypreprocessor <input.c> [-I<dir>]... [-o output.i] [-v]
```

### Argument Matrix

| Flag / Option | Type | Description |
| :--- | :--- | :--- |
| `<input.c>` | Parameter | **Required.** Target C source file to be preprocessed. |
| `-I<dir>` | Option | **Optional.** Adds a directory to the header include search path. Can be passed multiple times. |
| `-o output.i` | Option | **Optional.** Sets custom output path for preprocessed code. Defaults to `stdout` if omitted. |
| `-v` | Flag | **Optional.** Enables verbose logging, printing execution tracing and diagnostics. |

### Usage Examples

```bash
# Process a simple demonstration file to stdout
./mypreprocessor tests/demo.c

# Preprocess a file and output the results directly to an output file (.i)
./mypreprocessor tests/all_points.c -o tests/all_points.i

# Preprocess with multiple custom search directories and verbose debug output
./mypreprocessor tests/full_test.c -I./sysheaders -I./tests -o tests/full_test.i -v
```

## 6. Detailed Test Suite Matrix

The `tests/` directory contains tests for validating subsystem features:

| Test Input File | Primary Objective | Key Features Validated |
| :--- | :--- | :--- |
| `t1_comments.c` | Comment Removal | Single-line `//` and multi-line `/* ... */` comment stripping, string literal protection. |
| `t2_objmacro.c` | Macro Replacement | `#define` object macro registration and text token substitution. |
| `predef_test.c` | Built-in Environment | Expansion of predefined preprocessor macros like `__LINE__` and `__FILE__`. |
| `all_points.c` | Full Directives | Full suite of `#ifdef`, `#ifndef`, `#else`, `#undef`, and `#endif` conditional logic. |
| `acceptance.c` | End-to-End Pipeline | Full processing pipeline combining comment removal, includes, and macros. |
| `full_test.c` | Nested Integration | Complex multi-level include resolution via `helper.h` and `mathx.h`. |

## 7. Viva & Oral Exam Defense Preparation (`mypreprocessor_viva_Que.pdf`)

The included PDF file `mypreprocessor_viva_Que.pdf` serves as a study guide for viva examinations and technical reviews.

### Top Viva Concepts & Answers

1. **Why is simple `strstr` insufficient for macro expansion?**
   * *Answer:* Simple string substitution replaces substrings inside larger tokens (e.g., replacing `CAT` inside `CATERPILLAR` to produce `DOGERPILLAR`). `mypreprocessor` uses proper tokenization to respect word boundaries.

2. **How are comment markers inside strings handled?**
   * *Answer:* The DFA state machine tracks `STATE_STRING_LITERAL` upon encountering double quotes (`"`). While in this state, comment tokens (`//`, `/*`) are treated as literal character sequences rather than comment triggers.

3. **How does the inclusion engine prevent circular include crashes?**
   * *Answer:* An internal stack tracks active inclusion paths. If a header attempts to include a file already present on the active stack, an error or guard condition stops execution to prevent infinite recursion.

4. **What dynamic data structure is used for macro tracking?**
   * *Answer:* An in-memory dynamic symbol table (using linked list nodes or hash-indexed buckets) allows $O(1)$ to $O(N)$ lookup, addition, and removal of key-value replacement pairs.

## 8. License

This project is open-source software provided under the **MIT License**.
