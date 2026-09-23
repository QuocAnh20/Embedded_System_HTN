.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler
.global Default_Handler
.global _estack

.extern SystemInit
.extern main
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss

.section .isr_vector,"a",%progbits
.type g_pfnVectors,%object
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
.size g_pfnVectors, .-g_pfnVectors

.section .text.Reset_Handler
.type Reset_Handler,%function
.thumb_func
Reset_Handler:
    ldr r0, =_estack
    mov sp, r0

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

CopyData:
    cmp r1, r2
    bcs ZeroBss
    ldr r3, [r0]
    str r3, [r1]
    adds r0, r0, #4
    adds r1, r1, #4
    b CopyData

ZeroBss:
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0

ZeroLoop:
    cmp r1, r2
    bcs StartMain
    str r3, [r1]
    adds r1, r1, #4
    b ZeroLoop

StartMain:
    bl SystemInit
    bl main

InfiniteLoop:
    b InfiniteLoop

.size Reset_Handler, .-Reset_Handler

.thumb_func
Default_Handler:
    b Default_Handler

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak SVC_Handler
.thumb_set SVC_Handler, Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler

.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler

.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler
