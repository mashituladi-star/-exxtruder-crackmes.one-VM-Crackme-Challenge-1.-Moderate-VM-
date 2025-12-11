#ifndef VM_RUNTIME_H
#define VM_RUNTIME_H

#include <windows.h>
#include <cstdint>
#include <cstdio>

// Global VM runtime variables
struct VMRuntimeData {
    // Application type
    uint32_t app_type;
    
    // Command line arguments
    char** argv;
    int argc;
    char* cmd_line;
    
    // VM initialization flags
    uint32_t vm_initialized;
    uint32_t vm_initialization_lock;
    uint32_t vm_ready;
    
    // Exception handling
    PVOID original_exception_handler;
    
    // Standard streams
    FILE* stdin_stream;
    FILE* stdout_stream;
    FILE* stderr_stream;
};

// Get global VM runtime data
VMRuntimeData& get_vm_runtime();

// Runtime initialization functions
bool initialize_vm_runtime_critical();
void set_vm_exception_handler();
void set_vm_termination_handler();

// TLS (Thread Local Storage) callbacks
void tls_callback_0(uint64_t param1, uint32_t param2);
void tls_callback_1(uint64_t param1, uint32_t param2);

// VM exception handler
LONG WINAPI vm_exception_handler(PEXCEPTION_POINTERS exception_info);

// Math error handler for VM
int vm_math_error_handler(struct _exception* math_error_info);

// Process main VM logic
void vm_process_main_logic(uint8_t* buffer_ptr);

// Hash/checksum initialization
void initialize_vm_checksum();
void cleanup_vm_hash_algorithm();

#endif // VM_RUNTIME_H