#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <memory>

namespace Snow {
namespace PEGenerator {

// ============================================================================
// PE FILE STRUCTURES
// ============================================================================

#pragma pack(push, 1)

struct DOSHeader {
    uint16_t e_magic;    // Magic number (0x5A4D = "MZ")
    uint16_t e_cblp;
    uint16_t e_cp;
    uint16_t e_crlc;
    uint16_t e_cparhdr;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
 uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
 uint16_t e_cs;
    uint16_t e_lfarlc;
  uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
  uint16_t e_res2[10];
    uint32_t e_lfanew;   // File address of new exe header
};

struct PEHeader {
    uint32_t signature;  // PE signature (0x4550 = "PE\0\0")
};

struct COFFHeader {
    uint16_t machine;           // 0x8664 for x64
    uint16_t numberOfSections;
    uint32_t timeDateStamp;
    uint32_t pointerToSymbolTable;
    uint32_t numberOfSymbols;
    uint16_t sizeOfOptionalHeader;
    uint16_t characteristics;
};

struct OptionalHeader64 {
    uint16_t magic;             // 0x20b for PE32+
  uint8_t majorLinkerVersion;
uint8_t minorLinkerVersion;
    uint32_t sizeOfCode;
  uint32_t sizeOfInitializedData;
    uint32_t sizeOfUninitializedData;
uint32_t addressOfEntryPoint;
    uint32_t baseOfCode;
    uint64_t imageBase;
    uint32_t sectionAlignment;
    uint32_t fileAlignment;
    uint16_t majorOperatingSystemVersion;
    uint16_t minorOperatingSystemVersion;
    uint16_t majorImageVersion;
    uint16_t minorImageVersion;
    uint16_t majorSubsystemVersion;
    uint16_t minorSubsystemVersion;
    uint32_t win32VersionValue;
    uint32_t sizeOfImage;
    uint32_t sizeOfHeaders;
    uint32_t checkSum;
    uint16_t subsystem;
    uint16_t dllCharacteristics;
    uint64_t sizeOfStackReserve;
    uint64_t sizeOfStackCommit;
    uint64_t sizeOfHeapReserve;
    uint64_t sizeOfHeapCommit;
    uint32_t loaderFlags;
    uint32_t numberOfRvaAndSizes;
};

struct DataDirectory {
 uint32_t virtualAddress;
    uint32_t size;
};

struct SectionHeader {
    char name[8];
    uint32_t virtualSize;
    uint32_t virtualAddress;
    uint32_t sizeOfRawData;
    uint32_t pointerToRawData;
    uint32_t pointerToRelocations;
    uint32_t pointerToLinenumbers;
    uint16_t numberOfRelocations;
    uint16_t numberOfLinenumbers;
    uint32_t characteristics;
};

struct ImportDescriptor {
    uint32_t importLookupTableRVA;
    uint32_t timeDateStamp;
    uint32_t forwarderChain;
    uint32_t nameRVA;
    uint32_t importAddressTableRVA;
};

#pragma pack(pop)

// ============================================================================
// PE SECTION
// ============================================================================

class PESection {
public:
    PESection(const std::string& name, uint32_t characteristics);
    
    const std::string& GetName() const { return name_; }
    uint32_t GetCharacteristics() const { return characteristics_; }
    
    void AddData(const std::vector<uint8_t>& data);
    void AddData(const uint8_t* data, size_t size);
    void AddPadding(size_t count);
    
    const std::vector<uint8_t>& GetData() const { return data_; }
    size_t GetSize() const { return data_.size(); }
    
    void SetVirtualAddress(uint32_t va) { virtual_address_ = va; }
    uint32_t GetVirtualAddress() const { return virtual_address_; }
    
    void SetFileOffset(uint32_t offset) { file_offset_ = offset; }
 uint32_t GetFileOffset() const { return file_offset_; }

private:
    std::string name_;
    uint32_t characteristics_;
    std::vector<uint8_t> data_;
    uint32_t virtual_address_;
    uint32_t file_offset_;
};

// ============================================================================
// RELOCATION
// ============================================================================

struct Relocation {
    uint32_t virtual_address;
    uint16_t type;
    
    static constexpr uint16_t IMAGE_REL_BASED_ABSOLUTE = 0;
    static constexpr uint16_t IMAGE_REL_BASED_DIR64 = 10;
};

// ============================================================================
// IMPORT TABLE
// ============================================================================

class ImportTable {
public:
  ImportTable();
    
    void AddImport(const std::string& dll_name, const std::string& function_name);
    
    std::vector<uint8_t> GenerateImportData();
    
    uint32_t GetFunctionRVA(const std::string& dll_name, const std::string& function_name);

private:
    struct Import {
        std::string dll_name;
        std::vector<std::string> functions;
    };
    
  std::vector<Import> imports_;
};

// ============================================================================
// PE WRITER
// ============================================================================

class PEWriter {
public:
    PEWriter();
    
    // Add sections
    PESection* AddSection(const std::string& name, uint32_t characteristics);
    
    // Add code
    void AddCode(const std::vector<uint8_t>& code, uint32_t entry_point_offset = 0);
    
 // Add data
    void AddData(const std::vector<uint8_t>& data);
    
    // Add imports
    void AddImport(const std::string& dll_name, const std::string& function_name);
    
    // Add relocations
    void AddRelocation(uint32_t virtual_address, uint16_t type = Relocation::IMAGE_REL_BASED_DIR64);
    
    // Set entry point
    void SetEntryPoint(uint32_t rva);
    
    // Set image base
    void SetImageBase(uint64_t base);
    
    // Set subsystem (3 = console, 2 = GUI)
    void SetSubsystem(uint16_t subsystem);
    
    // Generate debug information
    void SetGenerateDebugInfo(bool generate);
    void AddDebugSymbol(const std::string& name, uint32_t address);
    
    // Write PE file
    bool Write(const std::string& filename);

private:
    std::vector<std::unique_ptr<PESection>> sections_;
    ImportTable import_table_;
    std::vector<Relocation> relocations_;
    
    uint32_t entry_point_;
    uint64_t image_base_;
    uint16_t subsystem_;
    bool generate_debug_info_;
    
    std::unordered_map<std::string, uint32_t> debug_symbols_;
    
    // Helper methods
    void WriteHeaders(std::ofstream& file);
    void WriteSections(std::ofstream& file);
  void WriteImportTable(std::ofstream& file);
    void WriteRelocations(std::ofstream& file);
    void WriteDebugInfo(std::ofstream& file);
 
    uint32_t AlignUp(uint32_t value, uint32_t alignment);
    void CalculateLayout();
};

// ============================================================================
// MACHINE CODE EMITTER
// ============================================================================

class MachineCodeEmitter {
public:
    MachineCodeEmitter();
  
  // Emit x64 instructions directly
  void EmitPrologue(std::vector<uint8_t>& code);
    void EmitEpilogue(std::vector<uint8_t>& code);
    
    // Arithmetic
    void EmitAdd(std::vector<uint8_t>& code, int dst_reg, int src_reg);
    void EmitSub(std::vector<uint8_t>& code, int dst_reg, int src_reg);
    void EmitMul(std::vector<uint8_t>& code, int dst_reg, int src_reg);
    void EmitDiv(std::vector<uint8_t>& code, int reg);
    
    // Move
 void EmitMov(std::vector<uint8_t>& code, int dst_reg, int src_reg);
    void EmitMovImm(std::vector<uint8_t>& code, int reg, int64_t immediate);
    
    // Memory
    void EmitLoad(std::vector<uint8_t>& code, int dst_reg, int base_reg, int32_t offset);
    void EmitStore(std::vector<uint8_t>& code, int base_reg, int32_t offset, int src_reg);
    
    // Control flow
    void EmitCall(std::vector<uint8_t>& code, int32_t offset);
    void EmitRet(std::vector<uint8_t>& code);
    void EmitJmp(std::vector<uint8_t>& code, int32_t offset);
    void EmitJe(std::vector<uint8_t>& code, int32_t offset);
    void EmitJne(std::vector<uint8_t>& code, int32_t offset);
    
    // SIMD instructions
    void EmitVectorAdd(std::vector<uint8_t>& code, int dst_xmm, int src1_xmm, int src2_xmm);
    void EmitVectorMul(std::vector<uint8_t>& code, int dst_xmm, int src1_xmm, int src2_xmm);
    
    // Helper: get register encoding
    static uint8_t GetRegisterEncoding(int reg);

private:
    void EmitREX(std::vector<uint8_t>& code, bool w, bool r, bool x, bool b);
    void EmitModRM(std::vector<uint8_t>& code, uint8_t mod, uint8_t reg, uint8_t rm);
    void EmitSIB(std::vector<uint8_t>& code, uint8_t scale, uint8_t index, uint8_t base);
};

// ============================================================================
// AOT COMPILER
// ============================================================================

class AOTCompiler {
public:
    AOTCompiler();
    
    // Compile SSA module to PE executable
    bool CompileToExecutable(const SSA::SSAModule& module, const std::string& output_path);
    
    // Embed metadata (CIAM tags, etc.)
    void SetEmbedMetadata(bool embed);
  
    // Generate debug symbols
    void SetGenerateDebugSymbols(bool generate);
    
private:
PEWriter pe_writer_;
    MachineCodeEmitter code_emitter_;
    bool embed_metadata_;
    bool generate_debug_symbols_;
  
    std::vector<uint8_t> GenerateCodeFromSSA(const SSA::SSAModule& module);
    void EmitFunction(const SSA::SSAFunction& func, std::vector<uint8_t>& code);
    void EmitInstruction(const SSA::SSAInstruction& instr, std::vector<uint8_t>& code);
};

} // namespace PEGenerator
} // namespace Snow
