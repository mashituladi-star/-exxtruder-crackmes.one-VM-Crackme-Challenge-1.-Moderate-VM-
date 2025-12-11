#include "../include/vm_runtime.h"
#include "../include/vm_core.h"
#include <cstring>
#include <cstdlib>
#include <process.h>

// Global runtime data
static VMRuntimeData g_vm_runtime = {0};

VMRuntimeData& get_vm_runtime() {
    return g_vm_runtime;
}

// TLS callback structures
#pragma section(".CRT$XLA", long, read)
#pragma section(".CRT$XLZ", long, read)

__declspec(allocate(".CRT$XLA")) PIMAGE_TLS_CALLBACK tls_callback_0_ptr = 
    reinterpret_cast<PIMAGE_TLS_CALLBACK>(tls_callback_0);
__declspec(allocate(".CRT$XLZ")) PIMAGE_TLS_CALLBACK tls_callback_1_ptr = 
    reinterpret_cast<PIMAGE_TLS_CALLBACK>(tls_callback_1);

bool initialize_vm_runtime_critical() {
    // Initialize critical section for VM
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&g_vm_runtime));
    
    // Set up exception handling
    g_vm_runtime.original_exception_handler = 
        SetUnhandledExceptionFilter(vm_exception_handler);
    
    // Initialize standard streams
    g_vm_runtime.stdin_stream = stdin;
    g_vm_runtime.stdout_stream = stdout;
    g_vm_runtime.stderr_stream = stderr;
    
    return true;
}

void set_vm_exception_handler() {
    // Already set in initialize_vm_runtime_critical
}

void set_vm_termination_handler() {
    // Set termination handlers if needed
    atexit([]() {
        // Cleanup code
    });
}

LONG WINAPI vm_exception_handler(PEXCEPTION_POINTERS exception_info) {
    if (!exception_info || !exception_info->ExceptionRecord) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    
    DWORD exception_code = exception_info->ExceptionRecord->ExceptionCode;
    
    // Handle specific VM exceptions
    switch (exception_code) {
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        case EXCEPTION_DATATYPE_MISALIGNMENT:
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_IN_PAGE_ERROR:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_OVERFLOW:
        case EXCEPTION_INVALID_DISPOSITION:
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_SINGLE_STEP:
        case EXCEPTION_STACK_OVERFLOW:
            // VM should handle these internally
            return EXCEPTION_EXECUTE_HANDLER;
            
        default:
            // Pass to original handler
            if (g_vm_runtime.original_exception_handler) {
                return g_vm_runtime.original_exception_handler(exception_info);
            }
            return EXCEPTION_CONTINUE_SEARCH;
    }
}

int vm_math_error_handler(struct _exception* math_error_info) {
    if (!math_error_info) {
        return 0;
    }
    
    const char* error_description = "Unknown math error";
    
    switch (math_error_info->type) {
        case _DOMAIN:
            error_description = "Argument domain error (DOMAIN)";
            break;
        case _SING:
            error_description = "Argument singularity (SIGN)";
            break;
        case _OVERFLOW:
            error_description = "Overflow range error (OVERFLOW)";
            break;
        case _UNDERFLOW:
            error_description = "The result is too small to be represented (UNDERFLOW)";
            break;
        case _TLOSS:
            error_description = "Total loss of significance (TLOSS)";
            break;
        case _PLOSS:
            error_description = "Partial loss of significance (PLOSS)";
            break;
    }
    
    fprintf(stderr, "_matherr(): %s in %s(%g, %g) (retval=%g)\n",
            error_description,
            math_error_info->name,
            math_error_info->arg2,
            math_error_info->arg1,
            math_error_info->retval);
    
    return 1;
}

void tls_callback_0(uint64_t param1, uint32_t param2) {
    // TLS callback for VM initialization
    if (param2 == 1) {
        // DLL_PROCESS_ATTACH
        initialize_vm_runtime_critical();
    }
}

void tls_callback_1(uint64_t param1, uint32_t param2) {
    // Additional TLS callback
    if (param2 == 3 || param2 == 0) {
        // DLL_THREAD_ATTACH or DLL_PROCESS_DETACH
        // Additional initialization/cleanup
    }
}

void initialize_vm_checksum() {
    // Initialize checksum or hash algorithm
    // This would initialize CRC32 or similar checksum
    static bool initialized = false;
    
    if (!initialized) {
        // Initialize hash algorithm tables
        // Set cleanup function
        atexit(cleanup_vm_hash_algorithm);
        initialized = true;
    }
}

void cleanup_vm_hash_algorithm() {
    // Cleanup hash algorithm resources
}