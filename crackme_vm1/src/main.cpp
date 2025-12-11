#include "vm_core.h"
#include "vm_runtime.h"
#include <iostream>
#include <cstring>

// Main VM program data
static const uint8_t VM_PROGRAM_DATA[] = {
    0x00, 0x01, 0x07, 0x64, 0x00, 0xFF, 0xAF, 0x4F, 0xE8, 
    0x80, 0xFF, 0xAF, 0x00, 0xFA, 0x64, 0x9F
};

// Main entry point for console application
int main(int argc, char* argv[]) {
    try {
        // Initialize VM runtime
        vm_initialize_console_app();
        
        // Create virtual machine
        VirtualMachine vm(0x3404);
        vm.initialize();
        
        // Load program into VM memory
        // We need to implement proper program loading
        
        std::cout << "Virtual Machine Initialized" << std::endl;
        std::cout << "Memory size: 0x3404 bytes" << std::endl;
        std::cout << "Addressable memory: 8192 x 13-bit words" << std::endl;
        
        // Check initial stack pointer
        uint16_t initial_sp = vm.read_memory(0x1FFE);
        std::cout << "Initial stack pointer: 0x" << std::hex << initial_sp << std::endl;
        
        // Main processing loop
        // vm_process_main_logic(vm.get_memory_buffer());
        
        // Execute VM
        vm.execute();
        
        std::cout << "VM execution completed" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "VM Error: " << e.what() << std::endl;
        return 1;
    }
}

// Entry point wrapper for different application types
extern "C" void entry(uint64_t param1, char** param2, uint64_t param3, void* param4) {
    // Set application type to GUI
    get_vm_runtime().app_type = 0;  // GUI mode
    
    // Call main initialization
    main(0, nullptr);
}

// Console application entry
extern "C" void vm_initialize_console_app_wrapper(uint64_t param1, char** param2, 
                                                  uint64_t param3, void* param4) {
    // Set application type to console
    get_vm_runtime().app_type = 1;  // Console mode
    
    // Call main initialization
    main(0, nullptr);
}