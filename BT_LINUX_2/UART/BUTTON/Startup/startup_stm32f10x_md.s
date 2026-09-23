.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler

/* định nghĩa kích thước Stack và Heap */
.equ Stack_Size, 0x00000400
.section .stack, "aw", %progbits
.align 3
Stack_Mem:
    .space Stack_Size
__initial_sp:

.section .heap, "aw", %progbits
.align 3
__heap_start:
.heap_size:
    .space 0x00000200
__heap_end:

.section .text.isr_vector
.align 2
g_pfnVectors:
    .word   __initial_sp
    .word   Reset_Handler
    .word   NMI_Handler
    .word   HardFault_Handler
    .word   MemManage_Handler
    .word   BusFault_Handler
    .word   UsageFault_Handler
    .word   0
    .word   0
    .word   0
    .word   0
    .word   SVC_Handler
    .word   DebugMon_Handler
    .word   0
    .word   PendSV_Handler
    .word   SysTick_Handler

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Sao chép dữ liệu từ Flash sang RAM nếu cần, hoặc nhảy thẳng đến main */
    ldr   r0, =__initial_sp
    mov   sp, r0
    bl    main
_infinite_loop:
    b     _infinite_loop

.size Reset_Handler, .-Reset_Handler

.macro def_default_handler handler_name
.weak \handler_name
.type \handler_name, %function
\handler_name:
    b   .
.endm

def_default_handler NMI_Handler
def_default_handler HardFault_Handler
def_default_handler MemManage_Handler
def_default_handler BusFault_Handler
def_default_handler UsageFault_Handler
def_default_handler SVC_Handler
def_default_handler DebugMon_Handler
def_default_handler PendSV_Handler
def_default_handler SysTick_Handler
