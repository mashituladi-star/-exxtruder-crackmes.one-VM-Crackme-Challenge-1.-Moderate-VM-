#ifndef VM_CORE_H
#define VM_CORE_H

#include <windows.h>
#include <cstdint>
#include <cstdlib>

// VM core constants
constexpr uint16_t VM_MEMORY_SIZE = 0x2000;  // 8192 addresses
constexpr uint16_t VM_STACK_POINTER = 0x1FFE;
constexpr uint16_t VM_INSTRUCTION_POINTER = 0x1FFF;

// VM status flags
struct VMStatusFlags {
    bool flag_carry : 1;     // C flag (0x3402)
    bool flag_zero : 1;       // Z flag (0x3400)
    bool flag_sign : 1;       // S flag (0x3401)
    bool flag_overflow : 1;   // V flag (0x3403)
};

// Virtual Machine core structure
class VirtualMachine {
private:
    uint8_t* memory_buffer;
    uint32_t buffer_size;
    VMStatusFlags status_flags;
    
public:
    VirtualMachine(uint32_t buffer_size = 0x3404);
    ~VirtualMachine();
    
    void initialize();
    void execute();
    void reset();
    
    // Memory access
    uint16_t read_memory(uint16_t address);
    void write_memory(uint16_t address, uint16_t value);
    
    // Stack operations
    void push(uint16_t value);
    uint16_t pop();
    
    // Flag accessors
    bool get_carry_flag() const { return status_flags.flag_carry; }
    bool get_zero_flag() const { return status_flags.flag_zero; }
    bool get_sign_flag() const { return status_flags.flag_sign; }
    bool get_overflow_flag() const { return status_flags.flag_overflow; }
    
    void set_carry_flag(bool value) { status_flags.flag_carry = value; }
    void set_zero_flag(bool value) { status_flags.flag_zero = value; }
    void set_sign_flag(bool value) { status_flags.flag_sign = value; }
    void set_overflow_flag(bool value) { status_flags.flag_overflow = value; }
};

// Application type for VM initialization
enum class ApplicationType {
    CONSOLE = 1,
    GUI = 2,
    UNKNOWN = 0
};

// Global VM initialization functions
void initialize_virtual_machine_runtime();
void vm_initialize_console_app();
void vm_initialize_gui_app();

#endif // VM_CORE_H