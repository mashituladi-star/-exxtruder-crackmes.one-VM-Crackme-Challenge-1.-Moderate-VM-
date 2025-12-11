#include "../include/vm_instructions.h"
#include "../include/vm_memory.h"
#include <cstring>

VMInstruction VMInstruction::decode(uint16_t instruction) {
    VMInstruction decoded;
    
    // Extract fields from instruction word
    // Format: OOOOOOOO OOMMDDDD (9-bit opcode, 2-bit mode1, 2-bit mode2)
    decoded.opcode = (instruction >> 4) & 0x1FF;  // 9 bits
    decoded.mode_dst = (instruction >> 2) & 0x3;   // 2 bits
    decoded.mode_src = instruction & 0x3;          // 2 bits
    
    return decoded;
}

uint16_t VMInstruction::encode() const {
    uint16_t encoded = 0;
    
    encoded |= (opcode & 0x1FF) << 4;
    encoded |= (mode_dst & 0x3) << 2;
    encoded |= (mode_src & 0x3);
    
    return encoded;
}

// Basic instruction executor implementation
class BasicInstructionExecutor : public InstructionExecutor {
public:
    bool execute(VMOpcode opcode, 
                uint16_t operand1, 
                uint16_t operand2,
                AddressingMode mode1,
                AddressingMode mode2,
                ExecutionContext& context) override {
        
        switch (opcode) {
            case VMOpcode::ADD: {
                uint32_t result = operand1 + operand2;
                context.status_flags[0] = (result & 0x2000) != 0;  // Sign flag
                context.status_flags[1] = (result & 0x1FFF) == 0;  // Zero flag
                context.status_flags[2] = (result > 0x1FFF);       // Carry flag
                
                // Overflow detection for signed addition
                bool op1_sign = (operand1 & 0x1000) != 0;
                bool op2_sign = (operand2 & 0x1000) != 0;
                bool res_sign = (result & 0x1000) != 0;
                context.status_flags[3] = (op1_sign == op2_sign) && (op1_sign != res_sign);
                
                // Write result
                VMMemoryManager::write_buffer_value(context.memory, operand1, 
                                                   static_cast<uint16_t>(result & 0x1FFF));
                break;
            }
            
            case VMOpcode::SUB: {
                uint16_t result = operand1 - operand2;
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = operand1 < operand2;     // Carry flag
                
                // Overflow detection for signed subtraction
                bool op1_sign = (operand1 & 0x1000) != 0;
                bool op2_sign = (operand2 & 0x1000) != 0;
                bool res_sign = (result & 0x1000) != 0;
                context.status_flags[3] = (op1_sign != op2_sign) && (op1_sign != res_sign);
                
                // Write result
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::AND: {
                uint16_t result = operand1 & operand2;
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = false;                   // Clear carry
                context.status_flags[3] = false;                   // Clear overflow
                
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::OR: {
                uint16_t result = operand1 | operand2;
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = false;                   // Clear carry
                context.status_flags[3] = false;                   // Clear overflow
                
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::XOR: {
                uint16_t result = operand1 ^ operand2;
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = false;                   // Clear carry
                context.status_flags[3] = false;                   // Clear overflow
                
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::NOT: {
                uint16_t result = ~operand1 & 0x1FFF;
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = false;                   // Clear carry
                context.status_flags[3] = false;                   // Clear overflow
                
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::CMP: {
                uint16_t result = operand1 - operand2;
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = operand1 < operand2;     // Carry flag
                
                // Overflow detection for signed comparison
                bool op1_sign = (operand1 & 0x1000) != 0;
                bool op2_sign = (operand2 & 0x1000) != 0;
                bool res_sign = (result & 0x1000) != 0;
                context.status_flags[3] = (op1_sign != op2_sign) && (op1_sign != res_sign);
                break;
            }
            
            case VMOpcode::SHL: {
                bool carry = (operand1 & 0x1000) != 0;
                uint16_t result = (operand1 << 1) & 0x1FFF;
                
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = carry;                   // Carry flag from MSB
                context.status_flags[3] = false;                   // Clear overflow
                
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::SHR: {
                bool carry = (operand1 & 1) != 0;
                uint16_t result = operand1 >> 1;
                
                context.status_flags[0] = (result & 0x1000) != 0;  // Sign flag
                context.status_flags[1] = result == 0;             // Zero flag
                context.status_flags[2] = carry;                   // Carry flag from LSB
                context.status_flags[3] = false;                   // Clear overflow
                
                VMMemoryManager::write_buffer_value(context.memory, operand1, result);
                break;
            }
            
            case VMOpcode::JMP: {
                context.ip = operand1 & 0x1FFF;
                return true;  // Continue execution
            }
            
            case VMOpcode::JZ: {
                if (context.status_flags[1]) {  // Zero flag
                    context.ip = operand1 & 0x1FFF;
                }
                return true;
            }
            
            case VMOpcode::JNZ: {
                if (!context.status_flags[1]) {  // Not zero
                    context.ip = operand1 & 0x1FFF;
                }
                return true;
            }
            
            case VMOpcode::HALT: {
                return false;  // Stop execution
            }
            
            default:
                // Unimplemented opcode
                return false;
        }
        
        return true;
    }
};