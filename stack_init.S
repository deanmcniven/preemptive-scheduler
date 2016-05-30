.extern     initial_r0
.extern     orig_spl
.extern     orig_sph

.MACRO      STACK_INIT                      ; Args: <SP_addr> <PC_addr>
            STS         initial_r0, r0      ; Store r0

            IN          r0, SPL             ; Store current SP
            STS         orig_spl, r0
            IN          r0, SPH
            STS         orig_sph, r0

            LDI         r0, low(@0)         ; Set SP
            OUT         SPL, r0
            LDI         r0, high(@0)
            OUT         SPH, r0

            LDI         r0, low(@1)         ; Push PC_L ret address to stack
            PUSH        r0
            LDI         r0, high(@1)        ; Push PC_H ret address to stack
            PUSH        r0

            LDI         r0, 0x00            ; Zero R0
            PUSH        r0                  ; Push to stack value for r0 - r31
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0
            PUSH        r0                  ; Push to stack value for SREG

            LDS         r0, orig_spl        ; Restore SP
            OUT         SPL, r0
            LDS         r0, orig_sph
            OUT         SPH, r0

            LDS         r0, initial_r0      ; Restore R0
.ENDM   
