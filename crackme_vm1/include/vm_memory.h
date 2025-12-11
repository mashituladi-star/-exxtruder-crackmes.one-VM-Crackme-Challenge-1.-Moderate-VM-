#ifndef VM_MEMORY_H
#define VM_MEMORY_H

#include <cstdint>

// Memory buffer operations for 13-bit packed values
class VMMemoryManager {
public:
    // Read a 13-bit value from packed memory buffer
    static uint16_t read_buffer_value(uint8_t* buffer_ptr, uint16_t address);
    
    // Write a 13-bit value to packed memory buffer
    static void write_buffer_value(uint8_t* buffer_ptr, uint16_t address, uint16_t value);
    
    // Calculate byte offset for 13-bit packed addressing
    static size_t calculate_byte_offset(uint16_t address);
    
    // Calculate bit offset for 13-bit packed addressing
    static uint8_t calculate_bit_offset(uint16_t address);
};

// Addressing modes for VM instructions
enum class AddressingMode : char {
    DIRECT = 0,      // Direct addressing
    INDIRECT = 1,    // Single indirect
    DOUBLE_INDIRECT = 2,  // Double indirect
    TRIPLE_INDIRECT = 3   // Triple indirect
};

// Operand resolution functions
class OperandResolver {
public:
    static uint16_t resolve_operand_address(uint8_t* buffer_ptr, 
                                           uint16_t base_address, 
                                           AddressingMode mode);
    
    static uint16_t get_pointer_value(uint8_t* buffer_ptr, 
                                     uint16_t base_address, 
                                     AddressingMode mode);
};

#endif // VM_MEMORY_H