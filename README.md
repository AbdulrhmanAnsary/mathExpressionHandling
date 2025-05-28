# Math Expression Converter

## Overview
Math Expression Converter is a C++ program that converts mathematical expressions between different notations (infix, prefix, and postfix) and evaluates them.
This project aims to build a solid foundation for handling mathematical expressions efficiently and serves as a good example of how to use stacks in C++.

## MVP (Minimum Viable Product)
The MVP focuses on providing core functionality for converting and evaluating mathematical expressions while ensuring correctness and readability.

### Features of the MVP:
- **Convert infix to prefix:** Supports basic arithmetic operations (`+`, `-`, `*`, `/`, `**` for exponentiation).
- **Convert infix to postfix:** Ensures correct operator precedence.
- **Handles parentheses correctly:** Supports expressions with `()` for proper grouping.
- **Processes single-digit, multi-digit, and floating-point numbers for both conversion and evaluation:** Including formats like `3.14`, `.5`, and `10.`.
- **Error-free conversion and evaluation (for valid expressions):** Ensures correct parsing and maintains mathematical correctness.

### Floating-Point Number Support
- **Supported Formats:** The system correctly parses and evaluates floating-point numbers in common formats such as `12.34`, `0.78`, `.5` (equivalent to 0.5), and `100.` (equivalent to 100.0).
- **Error Handling for Malformed Floats:** Malformed floating-point numbers (e.g., `3.1.4` with multiple decimal points, or `1..2` with consecutive points) are not treated as valid single numbers. The tokenizer will typically separate these components. If a standalone `.` or an invalid segment (which is not a valid number or operator) is encountered during the conversion or evaluation process, it will usually result in an "Unknown token" error.

## Product Roadmap
- **Extend conversion support:**
  - Convert **prefix to postfix**
  - Convert **prefix to infix**
  - Convert **postfix to infix**
  - Convert **postfix to prefix**
- **Improve error handling:** Further refine detection and reporting for a wider range of invalid expressions.
- **Optimize performance:** Enhance efficiency for large expressions.

## How to run the tests
```bash
clang++ testRunner.cpp mathExpressionsHandling.cpp -o testRunner
```
