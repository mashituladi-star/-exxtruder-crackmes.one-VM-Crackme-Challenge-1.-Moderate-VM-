#ifndef VM_INSTRUCTIONS_H
#define VM_INSTRUCTIONS_H

#include <cstdint>
#include <cstdio>
#include "vm_memory.h"

// VM instruction opcodes (based on analysis)
enum class VMOpcode : uint16_t {
    // Arithmetic operations
    ADD = 0x03,
    SUB = 0x04,
    AND = 0x05,
    OR = 0x08,
    XOR = 0x09,
    NOT = 0x0A,
    INC = 0x06,
    DEC = 0x07,
    
    // Bit operations
    SHL = 0x0D,  // Shift left
    SHR = 0x0E,  // Shift right
    ROL = 0x0B,  // Rotate left
    ROR = 0x0C,  // Rotate right
    
    // Comparison
    CMP = 0x0F,
    
    // Control flow
    JMP = 0x10,
    JZ = 0x11,   // Jump if zero
    JNZ = 0x12,  // Jump if not zero
    JC = 0x13,   // Jump if carry
    JNC = 0x14,  // Jump if not carry
    JS = 0x15,   // Jump if sign
    JNS = 0x16,  // Jump if not sign
    JO = 0x17,   // Jump if overflow
    JNO = 0x18,  // Jump if not overflow
    JL = 0x19,   // Jump if less
    JG = 0x1A,   // Jump if greater
    JLE = 0x1B,  // Jump if less or equal
    JGE = 0x1C,  // Jump if greater or equal
    
    // I/O operations
    IN = 0x24,   // Input character
    OUT = 0x25,  // Output character
    IN_STR = 0x26,  // Input string
    IN_HEX = 0x27,  // Input hexadecimal values
    
    // Stack operations
    PUSH = 0x22,
    POP = 0x23,
    
    // Memory operations
    MOV = 0x01,  // Move
    XCHG = 0x02, // Exchange
    
    // Flag operations
    CLC = 0x1F,  // Clear carry
    STC = 0x20,  // Set carry
    CMC = 0x21,  // Complement carry
    
    // System operations
    HALT = 0x29,
    NOP = 0x28
};

// Instruction format structure
struct VMInstruction {
    uint16_t opcode : 9;      // 9-bit opcode (bits 12-4)
    uint8_t mode_dst : 2;     // 2-bit destination addressing mode (bits 3-2)
    uint8_t mode_src : 2;     // 2-bit source addressing mode (bits 1-0)
    
    // Helper method to extract from raw instruction word
    static VMInstruction decode(uint16_t instruction);
    
    // Helper method to encode to raw instruction word
    uint16_t encode() const;
};

// Instruction execution context
struct ExecutionContext {
    uint8_t* memory;
    uint16_t ip;  // Instruction pointer
    uint16_t sp;  // Stack pointer
    bool* status_flags;
};

// Instruction executor interface
class InstructionExecutor {
public:
    virtual bool execute(VMOpcode opcode, 
                        uint16_t operand1, 
                        uint16_t operand2,
                        AddressingMode mode1,
                        AddressingMode mode2,
                        ExecutionContext& context) = 0;
    virtual ~InstructionExecutor() = default;
};

#endif // VM_INSTRUCTIONS_H