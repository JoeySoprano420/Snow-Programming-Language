# ❄️ SNOW COMPILER - BUILD & TEST GUIDE

## ✅ Build Status: SUCCESS

The Snow Programming Language compiler has been successfully built!

---

## 📦 What Was Built

### Complete Compiler Pipeline

1. **Lexer** (Tokenization)
   - Base-12 number parsing
   - Time duration literals
   - Dictionary-driven keywords
   - Case-insensitive support

2. **Parser** (Syntax Analysis)
   - Recursive descent parser
   - Frame-based AST construction
 - Full expression support

3. **AST** (Abstract Syntax Tree)
   - All statement types
   - Expression nodes
   - Temporal constructs

4. **IR Generator** (Intermediate Representation)
   - Dodecagram-hybrid IR
   - CIAM-compatible format
   - Virtual register allocation

5. **CIAM Optimizer**
   - Constant folding
   - Dead code elimination
   - Peephole optimization
   - Loop unrolling framework

6. **Code Generator** (x86_64 Assembly)
   - NASM-compatible output
   - Windows PE target
   - Function prologue/epilogue
   - Full instruction set

7. **Runtime Library**
   - I/O functions (Say)
   - Time functions (Wait)
   - Dodecagram conversions
   - Memory management

---

## 🚀 Quick Start

### 1. Build the Compiler

```bash
# In Visual Studio
Build → Build Solution (Ctrl+Shift+B)

# Or via command line
msbuild "Snow P L.vcxproj" /p:Configuration=Release /p:Platform=x64
```

### 2. Run Your First Program

```bash
cd bin\Debug
"Snow P L.exe" ..\..\examples\hello.sno
```

### 3. Expected Output

```
  ❄️  SNOW PROGRAMMING LANGUAGE  ❄️
  Version 1.0 — Dodecagram Edition
  Motto: "Rinse and Reuse."

[Compiler] Starting compilation of: ..\..\examples\hello.sno

[Lexer] Tokenizing source code...
[Parser] Building AST...
[IRGen] Generating intermediate representation...
[CIAM Optimizer] Running optimization passes...
  Optimizing function: main
[CIAM Optimizer] Optimization complete.
[CodeGen] Generating x86_64 assembly...
[CodeGen] Code generation complete: output.asm

✓ Compilation successful!
  Output: output.asm

  To assemble and link (using NASM):
    nasm -f win64 output.asm -o output.obj
    link output.obj /SUBSYSTEM:CONSOLE /OUT:program.exe
```

---

## 🧪 Test All Examples

### Basic Examples

```bash
# Hello World
"Snow P L.exe" ..\..\examples\hello.sno -o hello.asm

# Dodecagram Numbers
"Snow P L.exe" ..\..\examples\dodecagram.sno -o dodecagram.asm -v

# Temporal Operations
"Snow P L.exe" ..\..\examples\temporal.sno -o temporal.asm

# Functions and Derivatives
"Snow P L.exe" ..\..\examples\functions.sno -o functions.asm

# Full Demo
"Snow P L.exe" ..\..\examples\demo.sno -o demo.asm -O2
```

### View IR Output

```bash
"Snow P L.exe" ..\..\examples\demo.sno -emit-ir
```

---

## 📊 Compiler Features Implemented

✅ **Core Language Features**
- [x] Dodecagram number system (base-12)
- [x] Duration literals (ns, ms, s, m, h)
- [x] Variables and expressions
- [x] Functions with parameters
- [x] Conditionals (if/else)
- [x] Loops (every statement)
- [x] Derivatives (d(expr))
- [x] Comments (# and ##...##)
- [x] Case-insensitive keywords
- [x] String literals

✅ **Compiler Infrastructure**
- [x] Complete lexer with lookahead
- [x] Recursive descent parser
- [x] AST construction
- [x] IR generation
- [x] CIAM optimization framework
- [x] x86_64 code generation
- [x] Runtime library
- [x] Error reporting
- [x] Command-line interface

✅ **Optimization Passes**
- [x] Constant folding
- [x] Dead code elimination
- [x] Peephole optimization
- [x] Loop unrolling framework

---

## 🎯 Next Steps

### To Create an Executable

1. **Install NASM** (Netwide Assembler)
   ```
   Download from: https://www.nasm.us/
   ```

2. **Assemble the Output**
   ```bash
   nasm -f win64 output.asm -o output.obj
   ```

3. **Link (using VS Link or MinGW)**
   ```bash
   link output.obj /SUBSYSTEM:CONSOLE /OUT:program.exe
   ```

### Advanced Usage

```bash
# Optimize aggressively
"Snow P L.exe" program.sno -O2 -o optimized.asm

# Disable optimization
"Snow P L.exe" program.sno -O0 -o unoptimized.asm

# Verbose output
"Snow P L.exe" program.sno -v

# Emit IR for debugging
"Snow P L.exe" program.sno -emit-ir

# Custom output file
"Snow P L.exe" program.sno -o myprogram.asm
```

---

## 📝 Writing Snow Programs

### Simple Example

Create `test.sno`:

```snow
## My First Snow Program ##

Fn = [main];

let x = 10;    # 12 in decimal
let y = 5;
let sum = x + y;

Say("x = ", x);
Say("y = ", y);
Say("Sum = ", sum);
```

Compile:

```bash
"Snow P L.exe" test.sno -o test.asm -v
```

---

## 🔍 Troubleshooting

### Common Issues

**Issue**: Cannot find input file
- **Solution**: Use relative or absolute paths
- **Example**: `"Snow P L.exe" "C:\path\to\program.sno"`

**Issue**: Parse errors
- **Solution**: Check syntax, semicolons, and colons
- **Tip**: Use `-v` for verbose output

**Issue**: IR/Codegen errors
- **Solution**: Try with `-emit-ir` to see intermediate representation

---

## 📚 Language Reference

### Keywords
```
Fn, let, if, else, while, for, every, parallel, and,
derive, wait, ret, break, continue, namespace, use, end, Say, over
```

### Operators
```
+, -, *, /      # Arithmetic
==, !=, <, >, <=, >= # Comparison
=   # Assignment
```

### Data Types
- **Numbers**: Base-12 by default (10 = 12 decimal)
- **Strings**: "Hello, Snow!"
- **Durations**: 100ms, 3bms, 10s

---

## 🎓 Learning Resources

### Example Programs

1. **hello.sno** - Basic output
2. **dodecagram.sno** - Number system
3. **temporal.sno** - Time operations
4. **functions.sno** - Function calls
5. **demo.sno** - Comprehensive demo

### Documentation

- See `README.md` for full language specification
- Check source comments for implementation details
- Explore AST node types in `src/AST/AST.h`

---

## ⚡ Performance Tips

1. **Use -O2** for production code
2. **Leverage base-12** for natural divisions
3. **Use durations** for precise timing
4. **Employ derivatives** for change-over-time

---

## 🐛 Known Limitations

1. Only generates assembly (not executable directly)
2. Limited standard library
3. No type checking (dynamic only)
4. Single-threaded execution
5. Windows x64 only

---

## 🎉 Success!

You've successfully built the Snow Programming Language compiler!

**Next Steps:**
1. Try compiling the example programs
2. Write your own Snow programs
3. Experiment with dodecagram numbers
4. Explore temporal operations

---

**Happy Coding in Snow! ❄️**

*"Rinse and Reuse — Count in Dozens, Not Digits."*
