#include "../include/vm_memory.h"
#include <cstring>

uint16_t VMMemoryManager::read_buffer_value(uint8_t* buffer_ptr, uint16_t address) {
    // Each address stores 13 bits
    // Calculate byte offset: address * 13 / 8
    size_t byte_offset = (static_cast<size_t>(address) * 13) >> 3;
    
    // Calculate bit offset: (address * 13) % 8
    uint8_t bit_offset = (static_cast<uint8_t>(address) * 13) & 7;
    
    // Read first byte
    uint16_t value = static_cast<uint16_t>(buffer_ptr[byte_offset] >> bit_offset) & 0xFF;
    
    // If we need bits from next byte
    if (bit_offset > 3) {  // -5 < bit_offset in original
        value |= static_cast<uint16_t>(buffer_ptr[byte_offset + 1]) 
                 << (8 - bit_offset) & 0x1FFF;
    }
    
    // If we need bits from third byte
    if (bit_offset > 3) {
        value |= static_cast<uint16_t>(buffer_ptr[byte_offset + 2]) 
                 << (16 - bit_offset) & 0x1FFF;
    }
    
    return value & 0x1FFF;
}

void VMMemoryManager::write_buffer_value(uint8_t* buffer_ptr, uint16_t address, uint16_t value) {
    // Mask value to 13 bits
    uint16_t masked_value = value & 0x1FFF;
    
    // Calculate byte and bit offsets
    size_t byte_offset = (static_cast<size_t>(address) * 13) >> 3;
    uint8_t bit_offset = (static_cast<uint8_t>(address) * 13) & 7;
    
    // Calculate how many bits fit in first byte
    uint8_t bits_in_first_byte = 8 - bit_offset;
    if (bits_in_first_byte > 13) {
        bits_in_first_byte = 13;
    }
    
    // Create mask for first byte
    uint8_t first_mask = (1 << bits_in_first_byte) - 1;
    
    // Write to first byte
    buffer_ptr[byte_offset] = (buffer_ptr[byte_offset] & ~(first_mask << bit_offset)) |
                             ((masked_value & first_mask) << bit_offset);
    
    // Calculate remaining bits
    uint8_t remaining_bits = 13 - bits_in_first_byte;
    
    if (remaining_bits > 0) {
        // Write to second byte
        uint8_t bits_in_second = (remaining_bits > 8) ? 8 : remaining_bits;
        uint8_t second_mask = (1 << bits_in_second) - 1;
        
        buffer_ptr[byte_offset + 1] = 
            (buffer_ptr[byte_offset + 1] & ~second_mask) |
            (second_mask & (masked_value >> bits_in_first_byte));
        
        // Write to third byte if needed
        if (remaining_bits > bits_in_second) {
            uint8_t bits_in_third = remaining_bits - bits_in_second;
            uint8_t third_mask = (1 << bits_in_third) - 1;
            
            buffer_ptr[byte_offset + 2] = 
                (buffer_ptr[byte_offset + 2] & ~third_mask) |
                (third_mask & (masked_value >> (bits_in_first_byte + bits_in_second)));
        }
    }
}

uint16_t OperandResolver::resolve_operand_address(uint8_t* buffer_ptr, 
                                                 uint16_t base_address, 
                                                 AddressingMode mode) {
    uint16_t resolved_address = base_address & 0x1FFF;
    
    switch (mode) {
        case AddressingMode::DIRECT:
            // Direct addressing, return as-is
            break;
            
        case AddressingMode::INDIRECT:
            // Single indirect: read address from memory
            resolved_address = VMMemoryManager::read_buffer_value(buffer_ptr, resolved_address);
            break;
            
        case AddressingMode::DOUBLE_INDIRECT:
            // Double indirect
            resolved_address = VMMemoryManager::read_buffer_value(buffer_ptr, resolved_address);
            resolved_address = VMMemoryManager::read_buffer_value(buffer_ptr, resolved_address);
            break;
            
        case AddressingMode::TRIPLE_INDIRECT:
            // Triple indirect
            resolved_address = VMMemoryManager::read_buffer_value(buffer_ptr, resolved_address);
            resolved_address = VMMemoryManager::read_buffer_value(buffer_ptr, resolved_address);
            resolved_address = VMMemoryManager::read_buffer_value(buffer_ptr, resolved_address);
            break;
            
        default:
            resolved_address = 0;
            break;
    }
    
    return resolved_address & 0x1FFF;
}

uint16_t OperandResolver::get_pointer_value(uint8_t* buffer_ptr, 
                                           uint16_t base_address, 
                                           AddressingMode mode) {
    uint16_t pointer_value = base_address & 0x1FFF;
    
    // Note: This function appears to have different handling for mode 1
    if (mode != AddressingMode::INDIRECT) {
        if (mode == AddressingMode::DOUBLE_INDIRECT) {
            pointer_value = VMMemoryManager::read_buffer_value(buffer_ptr, pointer_value);
        } else if (mode == AddressingMode::TRIPLE_INDIRECT) {
            pointer_value = VMMemoryManager::read_buffer_value(buffer_ptr, pointer_value);
            pointer_value = VMMemoryManager::read_buffer_value(buffer_ptr, pointer_value);
        } else {
            pointer_value = 0;
        }
    }
    
    return pointer_value & 0x1FFF;
}