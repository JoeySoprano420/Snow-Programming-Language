#include "Common/Types.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "IR/IRGenerator.h"
#include "Optimizer/Optimizer.h"
#include "CodeGen/CodeGenerator.h"
#include "Runtime/Runtime.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace Snow;

// ============================================================================
// SNOW COMPILER DRIVER
// ============================================================================

std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void PrintBanner() {
    std::cout << "\n";
    std::cout << "  ❄️  SNOW PROGRAMMING LANGUAGE  ❄️\n";
    std::cout << "  Version 1.0 — Dodecagram Edition\n";
    std::cout << "Motto: \"Rinse and Reuse.\"\n";
    std::cout << "\n";
}

void PrintUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <source.sno> [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -o <file>    Output file (default: output.asm)\n";
    std::cout << "  -O0          No optimization\n";
 std::cout << "  -O1          Basic optimization (default)\n";
    std::cout << "  -O2  Advanced optimization\n";
 std::cout << "  -emit-ir     Emit IR instead of assembly\n";
    std::cout << "  -v           Verbose output\n";
    std::cout << "  -h, --help   Show this help message\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    PrintBanner();
    
    // Parse command line arguments
    if (argc < 2) {
PrintUsage(argv[0]);
   return 1;
    }
    
 std::string input_file;
    std::string output_file = "output.asm";
    bool emit_ir = false;
  bool verbose = false;
    bool optimize = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
    if (arg == "-h" || arg == "--help") {
  PrintUsage(argv[0]);
   return 0;
      } else if (arg == "-o" && i + 1 < argc) {
     output_file = argv[++i];
        } else if (arg == "-O0") {
       optimize = false;
   } else if (arg == "-emit-ir") {
emit_ir = true;
        } else if (arg == "-v") {
 verbose = true;
    } else if (arg[0] != '-') {
     input_file = arg;
     }
    }
    
if (input_file.empty()) {
   std::cerr << "Error: No input file specified\n";
        return 1;
    }
 
  try {
        // ====================================================================
        // COMPILATION PIPELINE
  // ====================================================================
  
        std::cout << "[Compiler] Starting compilation of: " << input_file << "\n\n";
     
        // 1. Read source code
        std::string source = ReadFile(input_file);
   if (verbose) {
    std::cout << "[Source] Read " << source.length() << " bytes\n";
        }
     
  // 2. Lexical Analysis
  std::cout << "[Lexer] Tokenizing source code...\n";
        Lexer lexer(source, input_file);
 
        // 3. Parsing
  std::cout << "[Parser] Building AST...\n";
        Parser parser(lexer);
     auto program = parser.ParseProgram();
        
        if (!program) {
       std::cerr << "Error: Failed to parse program\n";
            return 1;
 }
        
   if (verbose) {
      std::cout << "[AST] Program root: " << program->ToString() << "\n";
   std::cout << "[AST] Statements: " << program->GetStatements().size() << "\n";
   }
    
  // 4. IR Generation
        std::cout << "[IRGen] Generating intermediate representation...\n";
     IRGenerator ir_gen;
IR::Module* module = ir_gen.Generate(*program);
 
     if (emit_ir) {
  std::cout << "\n[IR] Emitting IR:\n";
   module->Print();
        }
   
        // 5. Optimization (CIAM)
  if (optimize) {
       CIAMOptimizer optimizer;
  optimizer.Optimize(*module);
    }
     
   if (verbose && !emit_ir) {
std::cout << "\n[IR] Optimized IR:\n";
      module->Print();
    }

  // 6. Code Generation
     if (!emit_ir) {
std::cout << "[CodeGen] Generating x86_64 assembly...\n";
 CodeGenerator codegen;
       
       if (!codegen.Generate(*module, output_file)) {
std::cerr << "Error: Code generation failed\n";
    return 1;
        }
        }
        
  // Success!
        std::cout << "\n✓ Compilation successful!\n";
   if (!emit_ir) {
    std::cout << "  Output: " << output_file << "\n";
      std::cout << "\n  To assemble and link (using NASM):\n";
       std::cout << "    nasm -f win64 " << output_file << " -o output.obj\n";
std::cout << "    link output.obj /SUBSYSTEM:CONSOLE /OUT:program.exe\n";
        }
std::cout << "\n";
 
    } catch (const std::exception& e) {
     std::cerr << "\n❌ Compilation failed!\n";
    std::cerr << "Error: " << e.what() << "\n\n";
    return 1;
    }
    
    return 0;
}
