#include "../include/vm_core.h"
#include "../include/vm_memory.h"
#include "../include/vm_runtime.h"
#include <cstring>
#include <stdexcept>
#include <../include/vm_instructions.h>

// Global application type
ApplicationType g_app_type = ApplicationType::UNKNOWN;

// VM initialization flag
static bool g_vm_initialized = false;

VirtualMachine::VirtualMachine(uint32_t buffer_size) 
    : buffer_size(buffer_size), memory_buffer(nullptr) {
    status_flags = {false, false, false, false};
}

VirtualMachine::~VirtualMachine() {
    if (memory_buffer) {
        free(memory_buffer);
        memory_buffer = nullptr;
    }
}

void VirtualMachine::initialize() {
    if (memory_buffer) {
        free(memory_buffer);
    }
    
    memory_buffer = static_cast<uint8_t*>(calloc(buffer_size, 1));
    if (!memory_buffer) {
        throw std::runtime_error("Failed to allocate VM memory");
    }
    
    // Initialize stack pointer
    write_memory(VM_STACK_POINTER, VM_MEMORY_SIZE - 1);
    
    // Initialize instruction pointer
    write_memory(VM_INSTRUCTION_POINTER, 0);
    
    reset();
}

uint16_t VirtualMachine::read_memory(uint16_t address) {
    if (address >= VM_MEMORY_SIZE) {
        throw std::out_of_range("VM memory read out of bounds");
    }
    
    // Use the packed 13-bit read function
    return VMMemoryManager::read_buffer_value(memory_buffer, address);
}

void VirtualMachine::write_memory(uint16_t address, uint16_t value) {
    if (address >= VM_MEMORY_SIZE) {
        throw std::out_of_range("VM memory write out of bounds");
    }
    
    // Use the packed 13-bit write function
    VMMemoryManager::write_buffer_value(memory_buffer, address, value & 0x1FFF);
}

void VirtualMachine::push(uint16_t value) {
    uint16_t sp = read_memory(VM_STACK_POINTER);
    write_memory(sp, value);
    write_memory(VM_STACK_POINTER, (sp - 1) & 0x1FFF);
}

uint16_t VirtualMachine::pop() {
    uint16_t sp = read_memory(VM_STACK_POINTER);
    uint16_t value = read_memory((sp + 1) & 0x1FFF);
    write_memory(VM_STACK_POINTER, (sp + 1) & 0x1FFF);
    return value;
}

void VirtualMachine::reset() {
    status_flags.flag_carry = false;
    status_flags.flag_zero = false;
    status_flags.flag_sign = false;
    status_flags.flag_overflow = false;
}

void VirtualMachine::execute() {
    // Main VM execution loop
    while (true) {
        uint16_t ip = read_memory(VM_INSTRUCTION_POINTER);
        uint16_t instruction = read_memory(ip);
        
        // Update instruction pointer
        write_memory(VM_INSTRUCTION_POINTER, (ip + 1) & 0x1FFF);
        
        // Decode and execute instruction
        VMInstruction decoded = VMInstruction::decode(instruction);
        
        // TODO: Implement instruction execution
        // This would involve a large switch statement or lookup table
        
        if (static_cast<VMOpcode>(decoded.opcode) == VMOpcode::HALT) {
            break;
        }
    }
}

// Global VM initialization function
void initialize_virtual_machine_runtime() {
    if (g_vm_initialized) {
        return;
    }
    
    // Initialize runtime critical sections
    if (!initialize_vm_runtime_critical()) {
        throw std::runtime_error("Failed to initialize VM runtime");
    }
    
    // Set exception handlers
    set_vm_exception_handler();
    set_vm_termination_handler();
    
    // Initialize checksum/hash algorithm
    initialize_vm_checksum();
    
    g_vm_initialized = true;
}

void vm_initialize_console_app() {
    g_app_type = ApplicationType::CONSOLE;
    initialize_virtual_machine_runtime();
}

void vm_initialize_gui_app() {
    g_app_type = ApplicationType::GUI;
    initialize_virtual_machine_runtime();
}